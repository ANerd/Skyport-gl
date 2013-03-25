#include "GameStateService.h"
#include "entity/Billboard.h"
#include "MortarAnimation.h"

#define XOR(p1, p2) ((p1 || p2) && !(p1 && p2))

void GameStateService::Player::Update(const PlayerState &other)
{
    if(Name != other.Name)
        throw Error(Error::InvalidValue, "Updating name of player");

    //StatsDirty |= other.Health != Health || other.Score != Score || 
    //    XOR(other.Index == 0, Index == 0);
    //StateDirty |= other.Position != Position;
    Index = other.Index;
    Score = other.Score;
    if(IsDead && Health != 0)
    {
        IsDead = false;
        Spawned = true;
        Debug("Player spawning");
    }
    if(other.Health != Health)
    {
        Health = other.Health;
        PlayerNametag->Health.Set(Health/100.0f);
        if(Health == 0)
        {
            Died = true;
            IsDead = true;
        }
    }
    if(Position != other.Position)
    {
        Debug("Warning: Jumping on gamesate");
        Position = other.Position;
        VectorF2 pos(
                Hexmap::jOffset[X]*Position[X]+Hexmap::kOffset[X]*Position[Y],
                Hexmap::jOffset[Y]*Position[X]+Hexmap::kOffset[Y]*Position[Y]);
        PlayerMovable->Transform.Set(
                MatrixF4::Translation(VectorF4(pos[X],0.0,pos[Y])));
    }
}

GameStateService::GameStateService(MultiContainer *container, Hexmap *map,
        AssetRef<Program> playerProgram,
        AssetRef<Texture> figureTexture, AssetRef<Texture> laserTexture,
        AssetRef<Texture> mortarTexture, AssetRef<Texture> droidTexture, 
        AssetRef<Texture> explosionTexture,AssetRef<Texture> iconTexture,
        Camera *camera) 
    : myAnimations(this), myPlayerProgram(playerProgram), Turn(-1), 
    myContainer(container), myMap(map), myFigureTexture(figureTexture), 
    myActionCount(0), myActionCursor(0), myAnimatingDying(false), 
    myCamera(camera), myLaser(laserTexture), myMortar(mortarTexture), 
    myInMortar(false), myDroid(droidTexture), myDroidSequenceCounter(-1), 
    myDoExplode(false), myExplosion(explosionTexture), myIcon(iconTexture)
{
    RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
            static_cast<EventCallback>(&GameStateService::StateUpdate));
    myDonePin = RegisterOutPin(SkyportEventClass::GameState, "Done");
    mySoundPin = RegisterOutPin(SkyportEventClass::Sound, "Sound");
    myTitle.Color.Set(ColorRGBA(255,255,0,255));
    myTitle.Text.Set("Welcome");
    myTitle.Anchors.Set(Anchor::Top);
    myTitle.Size.Set(SizeF2(0.1,0.2));
    myTitle.Position.Set(VectorF2(0,0.1));

    mySubtitle.Color.Set(ColorRGBA(255,255,0,255));
    mySubtitle.Text.Set("Game will begin shortly");
    mySubtitle.Anchors.Set(Anchor::Bottom);
    mySubtitle.Size.Set(SizeF2(0.1,0.15));
    mySubtitle.Position.Set(VectorF2(0,0.1));

    myFader.Pass.Set(10);
    myFader.Fading.Set(1);
    container->AddChild(&myTitle);
    container->AddChild(&mySubtitle);
    container->AddChild(&myFader);

    for(int i = 0; i < 6; i++)
    {
        myBiexplosions[i].SetTexture(explosionTexture);
    }
}

GameStateService::~GameStateService()
{
    for(auto pit = Players.begin(); 
            pit != Players.end(); pit++)
    {
        delete pit->PlayerMovable;
        delete pit->PlayerVisual;
        delete pit->NametagMovable;
        delete pit->PlayerContainer;
        delete pit->PlayerNametag;
    }
}


VectorI2 DirectionToTileOffset(Direction dir)
{
    VectorI2 tileoff;
    switch(dir)
    {
        case Direction::None:
            break;
        case Direction::Up:
            tileoff = VectorI2(-1,-1);
            break;
        case Direction::Down:
            tileoff = VectorI2(1 , 1);
            break;
        case Direction::Left_Up:
            tileoff = VectorI2(0,-1);
            break;
        case Direction::Left_Down:
            tileoff = VectorI2(1,0);
            break;
        case Direction::Right_Up:
            tileoff = VectorI2(-1,0);
            break;
        case Direction::Right_Down:
        tileoff = VectorI2(0,1);
            break;
    }
    return tileoff;
} 

VectorF2 TileToPosition(VectorI2 tile)
{
    return Hexmap::jOffset * tile[X] + Hexmap::kOffset * tile[Y];
}

VectorF2 DirectionToOffset(Direction dir)
{
    return TileToPosition(DirectionToTileOffset(dir));
}
void GameStateService::SetCurrentPlayer()
{
    while(myCurrentPlayer->Index != 0)
    {
        myCurrentPlayer++;
        if(myCurrentPlayer == Players.end())
            myCurrentPlayer = Players.begin();
    }
    if(!myCurrentPlayer->IsDead)
        myCurrentPlayer->PlayerMovable->Transform.Get().GetTranslation(myCameraTarget);
}

void GameStateService::MoveCamera(real angle, real time, real dragTime)
{
    if(myAnimations.GetNonPermanentCount() == 0)
    {
        ForceMoveCamera(angle, time, dragTime);
    }
    else
    {
        Debug("Not moving camera, %d running", myAnimations.GetNonPermanentCount());
    }
}
void GameStateService::ForceMoveCamera(real angle, real time, real dragTime, real height)
{
    VectorF4 oldtarget;
    myCamMarkerMov.Transform.Get().GetTranslation(oldtarget);
    VectorF4 cam = myCameraTarget + MatrixF4::RotationY(angle)*VectorF4(0, height, -10);

    if((oldtarget - myCameraTarget).SquareLength() + (myOldCamera - cam).SquareLength()  > 0.1)
    {
        AnimationHelper::TranslationAnimationData *markdata =
            new AnimationHelper::TranslationAnimationData(
                    &myCamMarkerMov,
                    oldtarget, 
                    myCameraTarget,
                    time, AnimationHelper::SmoothCurve);
        myAnimations.AddAnimation(markdata);

        AnimationHelper::TranslationAnimationData *camdata =
            new AnimationHelper::TranslationAnimationData(
                    &myCamMov,
                    myOldCamera, 
                    cam,
                    time+dragTime, AnimationHelper::SmoothCurve);
        myAnimations.AddAnimation(camdata);
        myOldCamera = cam;
        Debug("Start movement");
    }
}

void GameStateService::Update(const GameState &state)
{
    VectorI2 mapSize = state.GetMap().GetSize();
    if(Turn == -1)
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Movable *mov = new Movable();
            Billboard *bill = new Billboard(myFigureTexture);
            bill->SetProgram(myPlayerProgram);
            Movable *nameMov = new Movable();
            MultiContainer *container = new MultiContainer();
            Nametag *nametag = new Nametag();
            bill->Offset.Set(VectorF2(0,0.65));
            nametag->Offset.Set(VectorF2(0,1.45));

            nametag->PlayerName.Set(pit->Name);
            nametag->Health.Set(pit->Health/100.0f);
            nametag->Visible.Set(false);

            mov->SetChild(container);
            container->AddChild(bill);
            container->AddChild(nameMov);
            nameMov->SetChild(nametag);
            myContainer->AddChild(mov);
            bill->ProgramState().SetUniform("Z", -0.05f);
            bill->ProgramState().SetUniform("FrameCount", VectorI2(16,7));
            bill->ProgramState().SetUniform("ColorKey", VectorF4(1.0,0.0,1.0,1.0));
            bill->ProgramState().SetUniform("Color", pit->Color);
            bill->ProgramState().SetUniform("Size", VectorF2(1.3,1.3));
            bill->Visible.Set(false);
            nametag->ProgramState().SetUniform("Size", VectorF2(1.6,0.2));
            Players.push_back(Player(i++,pit->Name,mov,bill,nameMov,
                        container, nametag));
            Players.back().Update(*pit);
        }
        myMap->Create(mapSize[X],mapSize[Y]);
        myCurrentPlayer = Players.begin();

        AnimationHelper::HideAnimationData *hanim = 
                new AnimationHelper::HideAnimationData(&mySubtitle, 6);
        hanim->Presistant = true;
        mySubtitleAnimation = myAnimations.AddAnimation(hanim);

        VectorF2 midpoint = TileToPosition(mapSize / 2);
        myDefaultLookat = VectorF4(midpoint[X], 0.0f, midpoint[Y]);
        myDefaultCamera = VectorF4(0,10,-10) + myDefaultLookat; //VectorF4(-midpoint[X], abs(midpoint[X] + midpoint[Y]), -midpoint[Y]);
        myCameraTarget = myDefaultLookat;
        myOldCamera = myDefaultCamera;

        myCamera->Near.Set(0.5);
        myCamera->Far.Set(200);
        myCamera->FOV.Set(3.14/4);

        myCamMov.Transform.Set(MatrixF4::Translation(myDefaultCamera));
        myCamMov.SetPointAt(&myCamMarker);
        myCamMov.InstanceId.Set(1);
        myCamMov.SetChild(myCamera);

        myCamMarkerMov.InstanceId.Set(2);
        myCamMarkerMov.Transform.Set(MatrixF4::Translation(myDefaultLookat));
        myCamMarkerMov.SetChild(&myCamMarkerC);

        myCamMarkerC.AddChild(&myCamMarker);

        myContainer->AddChild(&myCamMarkerMov);
        myContainer->AddChild(&myCamMov);

        real laserScale = Hexmap::jOffset.Length();
        myLaserBaseTransform = MatrixF4::RotationX(-Pi / 2) 
            * MatrixF4::Scale(VectorF4(laserScale, laserScale, laserScale, 1));
        myLaser.Visible.Set(false);
        myLaserMov.SetChild(&myLaser);
        myLaserMov.Transform.Set(myLaserBaseTransform);
        myContainer->AddChild(&myLaserMov);
        myLaser.Length.Set(8);

        myMortarMov.SetChild(&myMortar);
        myContainer->AddChild(&myMortarMov);
        myMortar.ProgramState().SetUniform("FrameCount", VectorI2(1,16));
        myMortar.ProgramState().SetUniform("Size", VectorF2(0.3,0.3));
        myMortar.Visible.Set(false);

        myContainer->AddChild(&myExplosionMov);
        myExplosionC.AddChild(&myExplosion);
        myExplosionMov.SetChild(&myExplosionC);
        myExplosion.ProgramState().SetUniform("FrameCount", VectorI2(16,1));
        myExplosion.ProgramState().SetUniform("Offset", VectorF2(0,0.5));
        myExplosion.ProgramState().SetUniform("Size", VectorF2(2,2));
        myExplosion.ProgramState().SetUniform("Z", -0.1f);
        myExplosion.Visible.Set(false);

        for(int i = 0; i < 6; i++)
        {
            Direction dir;
            switch(i)
            {
                case 0:
                    dir = Direction::Up;
                    break;
                case 1:
                    dir = Direction::Down;
                    break;
                case 2:
                    dir = Direction::Left_Up;
                    break;
                case 3:
                    dir = Direction::Left_Down;
                    break;
                case 4:
                    dir = Direction::Right_Up;
                    break;
                case 5:
                    dir = Direction::Right_Down;
                    break;

            }
            VectorF2 offset = DirectionToOffset(dir);

            myExplosionC.AddChild(myBiexplosionMoves + i);
            myBiexplosionMoves[i].SetChild(myBiexplosions + i);
            myBiexplosionMoves[i].Transform
                .Set(MatrixF4::Translation(VectorF4(offset[X], 0, offset[Y])));
            myBiexplosions[i].ProgramState().SetUniform("FrameCount", VectorI2(16,1));
            myBiexplosions[i].ProgramState().SetUniform("Offset", VectorF2(0,0.5));
            myBiexplosions[i].ProgramState().SetUniform("Size", VectorF2(1,1));
            myBiexplosions[i].ProgramState().SetUniform("Z", -0.1f);
            myBiexplosions[i].Visible.Set(false);
        }

        myDroidMov.SetChild(&myDroid);
        myContainer->AddChild(&myDroidMov);
        myDroid.Visible.Set(false);
        myDroid.ProgramState().SetUniform("Offset", VectorF2(0,0.5));
        myDroid.ProgramState().SetUniform("FrameCount", VectorI2(16,1));
        myDroid.ProgramState().SetUniform("Z", -0.8f);
        
        myIconMov.SetChild(&myIcon);
        myContainer->AddChild(&myIconMov);
        myIcon.ProgramState().SetUniform("Offset", VectorF2(0,2.5));
        myIcon.ProgramState().SetUniform("FrameCount", VectorI2(3,1));
        myIcon.ProgramState().SetUniform("Size", VectorF2(1.5,1.5));
        myIcon.Visible.Set(false);

        for(uint i = 0; i < MeteorCount; i++)
        {
            myMeteorMovs[i].SetChild(&myMeteors[i]);
            //myMeteorMovs[i].Transform.Set(MatrixF4::RotationZ(-0.463647609));
            MeteorAnimationData *adata = 
                new MeteorAnimationData(&myMeteorMovs[i], 2, 
                        AnimationHelper::LinearCurve);
            adata->CurrentTime = double(std::rand()) * 10 / double(RAND_MAX);
            adata->Finalize();
            myAnimations.AddAnimation(adata);

            AnimationHelper::TextureAnimationData *tedata =
                new AnimationHelper::TextureAnimationData(
                    &myMeteors[i], 16, X, 1.0,
                    AnimationHelper::LinearCurve);
            tedata->Delay = 10;
            tedata->Repeating = true;
            myAnimations.AddAnimation(tedata);

            myContainer->AddChild(myMeteorMovs + i);
        }

        FaderAnimationData *fdata = new FaderAnimationData(&myFader, 5, true, AnimationHelper::LinearCurve);
        myAnimations.AddAnimation(fdata);

        myStats.State.Set(state);
        myStats.Anchors.Set(Anchor::Bottom);
        myStats.Fill.Set(FillDirection::Width);
        myContainer->AddChild(&myStats);
    }
    else
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            uint oldscore = Players[i].Score;
            Players[i].Update(*pit);
            if(oldscore != Players[i].Score)
                myStats.State.Set(state);
            if(Players[i].GetDied())
            {
                myDyingPlayers.push_back(i);
            }
            i++;
        }
    }

    for(int j = 0;  j < mapSize[X]; j++)
    {
        for(int k = 0;  k < mapSize[Y]; k++)
        {
            myMap->SetTileType(j,k,state.GetMap()(j,k));
        }
    }
    if(myDyingPlayers.size() == 0)
    {
        SetCurrentPlayer();
        MoveCamera();
    }
    else
    {
        Debug("Someone died!");
    }

    if(Turn != state.GetTurn())
    {
        myActionCursor = 0;
    }

    myActionCount = state.GetActionCount();
    for(uint i = 0; i < myActionCount; i++)
    {
        myActionStates[i] = state.GetAction(i);
    }

    myTitle.Text.Set(state.GetTitle());
    if(state.GetSubtitle() != mySubtitle.Text.Get())
    {
        mySubtitle.Text.Set(state.GetSubtitle());
        myAnimations.ResetAnimation(mySubtitleAnimation);
    }

    PlayAnimation();

    Turn = state.GetTurn();
}

real DirectionToAngle(Direction dir)
{
    switch(dir)
    {
        case Direction::Up:
            return 3 * Pi / 2;
        case Direction::Down:
            return Pi / 2;
        case Direction::Left_Down:
            return Pi / 6;
        case Direction::Right_Down:
            return 5 * Pi / 6;
        case Direction::Right_Up:
            return 7 * Pi / 6;
        case Direction::Left_Up:
            return 11 * Pi / 6;
        default:
            Warning("Unknown laser direction.");
            return 0;
    }
}

void GameStateService::Explode(VectorF4 pos)
{
    AnimationHelper::HideAnimationData *hdata = 
        new AnimationHelper::HideAnimationData(&myExplosion, 1);

    AnimationHelper::TextureAnimationData *tdata = 
        new AnimationHelper::TextureAnimationData(
                &myExplosion, 16, X, 1, AnimationHelper::LinearCurve);


    myExplosionMov.Transform.Set(MatrixF4::Translation(pos));
    myExplosion.Visible.Set(true);
    myAnimations.AddAnimation(hdata);
    myAnimations.AddAnimation(tdata);

    for(int i = 0; i < 6; i++)
    {
        myBiexplosions[i].Visible.Set(true);
        AnimationHelper::HideAnimationData *hbdata = 
            new AnimationHelper::HideAnimationData(myBiexplosions + i, 1);

        AnimationHelper::TextureAnimationData *tbdata = 
            new AnimationHelper::TextureAnimationData(
                    myBiexplosions + i, 16, X, 1, AnimationHelper::LinearCurve);
        myAnimations.AddAnimation(hbdata);
        myAnimations.AddAnimation(tbdata);
    }
}

void DirectionToView(Direction dir, real &angle, bool &flip)
{
    switch(dir)
    {
        case Direction::Right_Down:
            angle = -Pi / 6;
            flip = false;
            break;
        case Direction::Right_Up:
            angle = Pi / 6;
            flip = false;
            break;
        case Direction::Left_Up:
            angle = -Pi / 6;
            flip = true;
            break;
        case Direction::Left_Down:
            angle = Pi / 6;
            flip = true;
            break;
        case Direction::Up:
            angle = Pi / 2;
            flip = false;
            break;
        case Direction::Down:
            angle = Pi / 2;
            flip = true;
            break;
        default:
            angle = 0;
            flip = false;
    }
}

void GameStateService::PlayAnimation()
{
    if(myAnimations.GetNonPermanentCount() == 0)
    {
        if(myDyingPlayers.size() != 0)
        {
            VectorF4 pos;
            Player &dyingplayer = Players[myDyingPlayers.back()];
            myDyingPlayers.pop_back();
            dyingplayer.PlayerMovable->Transform.Get()
                .GetTranslation(pos);
            VectorF4 camoff = pos - myCameraTarget;
            if(camoff.SquareLength() > 1)
            {
               myCameraTarget = pos;
                ForceMoveCamera();
            }
            else
            {
                AnimationHelper::TextureAnimationData *tedata =
                    new AnimationHelper::TextureAnimationData(
                        dyingplayer.PlayerVisual, 16, X, 1,
                        AnimationHelper::LinearCurve, 1);
                myAnimations.AddAnimation(tedata);
                
                AnimationHelper::HideAnimationData *hpdata =
                    new AnimationHelper::HideAnimationData(
                            dyingplayer.PlayerVisual, 1);
                myAnimations.AddAnimation(hpdata);
                dyingplayer.PlayerNametag->Visible.Set(false);
            }
            PlaySound(Sound::RobotDestruction);
        }
        else if(myAnimatingDying)
        {
            SetCurrentPlayer();
            MoveCamera();
            myAnimatingDying = false;
        }
        else if(myCurrentPlayer->GetSpawned())
        {
            myCurrentPlayer->PlayerVisual->ProgramState().SetUniform("Frame", VectorI2(0,4));
            AnimationHelper::TextureAnimationData *tedata =
                new AnimationHelper::TextureAnimationData(
                    myCurrentPlayer->PlayerVisual, 16, X, 1,
                    AnimationHelper::LinearCurve, 4);
            myAnimations.AddAnimation(tedata);
            myCurrentPlayer->PlayerVisual->Visible.Set(true);
            myCurrentPlayer->PlayerNametag->Visible.Set(true);
            PlaySound(Sound::RobotRespawn);
        }
        else if(myInMortar)
        {
            myMortar.Visible.Set(false);
            VectorF4 pos;
            myMortarMov.Transform.Get().GetTranslation(pos);
            if(myDoExplode)
            {
                Explode(pos);
                PlaySound(Sound::MotarImpact);
            }

            myInMortar = false;
            myActionCursor++;
        }
        else if(myDroidSequenceCounter != -1)
        {
            while(myDroidSequenceCounter < ActionState::MaxDroidCommands)
            {
                Direction dir = myActionStates[myActionCursor]
                    .GetCommands()[myDroidSequenceCounter];
                if(dir != Direction::None)
                {
                    VectorF4 pos;
                    myDroidMov.Transform.Get().GetTranslation(pos);

                    VectorF2 off = DirectionToOffset(dir);
                    AnimationHelper::TranslationAnimationData *trdata =
                        new AnimationHelper::TranslationAnimationData(
                            &myDroidMov, pos, 
                            VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]), 
                            1, AnimationHelper::SmoothCurve);
                    trdata->Delay = 1;
                    myAnimations.AddAnimation(trdata);

                    AnimationHelper::TextureAnimationData *tedata =
                        new AnimationHelper::TextureAnimationData(
                            &myDroid, 16, X, 1,
                            AnimationHelper::LinearCurve, 0);
                    tedata->Delay = 1;
                    myAnimations.AddAnimation(tedata);

                    real angle;
                    bool flip;
                    DirectionToView(dir, angle, flip);

                    if(flip)
                        myDroid.ProgramState().SetUniform("Flip", VectorF2(1,0));
                    else
                        myDroid.ProgramState().SetUniform("Flip", VectorF2(0,0));

                    myCameraTarget = VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]);
                    ForceMoveCamera(angle);
                    myDroidSequenceCounter++;
                    PlaySound(Sound::DroidStep);
                    return;
                }
                myDroidSequenceCounter++;
            }
            myActionCursor++;
            myDroidSequenceCounter = -1;

            myDroid.Visible.Set(false);

            VectorF4 pos;
            myDroidMov.Transform.Get().GetTranslation(pos);
            Explode(pos);

            PlaySound(Sound::DroidImpact);
        }
        else if(myActionCursor < myActionCount)
        {
            if(myCurrentPlayer->Index != 0)
                throw Error(Error::InvalidState, "Action on wrong player");
            switch(myActionStates[myActionCursor].GetAction())
            {
                case SkyportAction::Move:
                    {
                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);

                        VectorI2 tileoff = DirectionToTileOffset(
                                myActionStates[myActionCursor].GetDirection());
                        VectorF2 off = TileToPosition(tileoff);
                        myCurrentPlayer->Position += tileoff;
                        AnimationHelper::TranslationAnimationData *trdata =
                            new AnimationHelper::TranslationAnimationData(
                                myCurrentPlayer->PlayerMovable,
                                pos, 
                                VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]), 
                                1, AnimationHelper::SmoothCurve);
                        trdata->Delay = 1;
                        myAnimations.AddAnimation(trdata);

                        myCameraTarget = VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]);
                        real angle;
                        bool flip;
                        DirectionToView(
                                myActionStates[myActionCursor].GetDirection(), 
                                angle, flip);
                        ForceMoveCamera(angle, 1, 0.5, 4);

                        if(flip)
                            myCurrentPlayer->PlayerVisual->ProgramState().SetUniform("Flip", VectorF2(1,0));
                        else
                            myCurrentPlayer->PlayerVisual->ProgramState().SetUniform("Flip", VectorF2(0,0));
                        AnimationHelper::TextureAnimationData *tedata =
                            new AnimationHelper::TextureAnimationData(
                                myCurrentPlayer->PlayerVisual, 16, X, 1, //<-- "1" here is duration
                                AnimationHelper::LinearCurve, 2);
                        tedata->Delay = 1;
                        myAnimations.AddAnimation(tedata);
                        myActionCursor++;
                    }
                    break;
                case SkyportAction::Laser:
                    {
                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);
                        pos[Y] = 0.5;
                        MatrixF4 localtransform = MatrixF4::RotationY(
                                DirectionToAngle(myActionStates[myActionCursor].
                                    GetDirection()));
                        localtransform.SetTranslation(pos);
                        myLaserMov.Transform.Set(localtransform 
                                * myLaserBaseTransform);

                        VectorI2 offset = myActionStates[myActionCursor].GetCoordinate();
                        Debug("Laser offset: "+static_cast<std::string>(offset));
                        int length = std::max(abs(offset[X]), abs(offset[Y]));
                        VectorF2 off = TileToPosition(offset)/2;


                        myLaser.Length.Set(0);
                        myLaser.Visible.Set(true);
                        real rollSpeed = 0.3/8;
                        LaserAnimationData *ldata = 
                            new LaserAnimationData(&myLaser, length+0.5, 0.3/8, 
                                    AnimationHelper::LinearCurve);
                        ldata->Delay = 0.4;
                        myAnimations.AddAnimation(ldata);

                        AnimationHelper::TextureAnimationData *tedata =
                            new AnimationHelper::TextureAnimationData(
                                &myLaser, 16, Y, 1,
                                AnimationHelper::LinearCurve);
                        ldata->Delay = 0.4;
                        myAnimations.AddAnimation(tedata);

                        AnimationHelper::HideAnimationData *hdata = 
                            new AnimationHelper::HideAnimationData(&myLaser, 1);
                        myAnimations.AddAnimation(hdata);
                        myCameraTarget = VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]);

                        AnimationHelper::TextureAnimationData *ptedata =
                            new AnimationHelper::TextureAnimationData(
                                myCurrentPlayer->PlayerVisual, 16, X, 1,
                                AnimationHelper::LinearCurve, 6);
                        myAnimations.AddAnimation(ptedata);

                        real angle;
                        bool flip;
                        DirectionToView(
                                myActionStates[myActionCursor].GetDirection(), 
                                angle, flip);

                        if(flip)
                            myCurrentPlayer->PlayerVisual->ProgramState().SetUniform("Flip", VectorF2(1,0));
                        else
                            myCurrentPlayer->PlayerVisual->ProgramState().SetUniform("Flip", VectorF2(0,0));

                        ForceMoveCamera(angle, std::min(rollSpeed * (length + 0.5) + 0.2, 1.0), 0.1);

                        PlaySound(Sound::Laser, 1);

                        myActionCursor++;
                    }
                    break;
                case SkyportAction::Motar:
                    {
                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);
                        pos[Y] = 0;

                        VectorF2 target = TileToPosition(
                                myActionStates[myActionCursor].GetCoordinate()) 
                            + VectorF2(pos[X], pos[Z]);

                        myMortar.Visible.Set(true);

                        MortarAnimationData *mdata = 
                            new MortarAnimationData(&myMortarMov, 1.5, pos, 
                                    VectorF4(target[X], 0, target[Y]), 5,
                                    AnimationHelper::LinearCurve);
                        mdata->Delay = 0.4;
                        myAnimations.AddAnimation(mdata);
                        PlaySound(Sound::MotarFire);
                        PlaySound(Sound::MotarAir, 5);

                        VectorI2 targetTile = myCurrentPlayer->Position +
                            myActionStates[myActionCursor].GetCoordinate();
                        char t = myMap->GetTileType(targetTile[X],targetTile[Y]);
                        Debug(std::string("Type is: ")+t);
                        myDoExplode = !(t == 'V' || t == 'O');
                        
                        AnimationHelper::TextureAnimationData *tedata =
                            new AnimationHelper::TextureAnimationData(
                                myCurrentPlayer->PlayerVisual, 16, X, 1,
                                AnimationHelper::LinearCurve, 5);
                        myAnimations.AddAnimation(tedata);
                        ForceMoveCamera();

                        myInMortar = true;
                    }
                    break;
                case SkyportAction::Droid:
                    {
                        PlaySound(Sound::DroidFire);
                        myDroid.Visible.Set(true);

                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);
                        pos[Y] = 0;
                        myDroidMov.Transform.Set(MatrixF4::Translation(pos));
                        myDroidSequenceCounter = 0;
                    }
                    break;
                case SkyportAction::Mine:
                    {
                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);
                        myIconMov.Transform.Set(MatrixF4::Translation(pos));
                        int frame = -1;
                        VectorI2 tile = myCurrentPlayer->Position;
                        char t = myMap->GetTileType(tile[X],tile[Y]);
                        if(t == 'E')
                            frame = 0;
                        else if(t == 'R')
                            frame = 1;
                        else if(t == 'C')
                            frame = 2;
                        if(frame != -1)
                        {
                            myIcon.ProgramState().SetUniform("Frame", VectorI2(frame, 0));
                            myIcon.Visible.Set(true);
                            AnimationHelper::HideAnimationData *hdata = 
                                new AnimationHelper::HideAnimationData(&myIcon, 1);
                            myAnimations.AddAnimation(hdata);

                            AnimationHelper::TextureAnimationData *tedata =
                                new AnimationHelper::TextureAnimationData(
                                    myCurrentPlayer->PlayerVisual, 16, X, 1,
                                    AnimationHelper::LinearCurve, 3);
                            myAnimations.AddAnimation(tedata);
                            PlaySound(Sound::RobotMining);
                        }
                        myActionCursor++;
                    }
                    break;
                default:
                    Debug("Action not implemented, may hang.");
                    myActionCursor++;
            }
        }
        else
        {
            Event nevent(SkyportEventClass::GameState, 
                    GameStateEventCodes::StateProcessed, this);
            myDonePin.Send(nevent);
        }
    }
}

void GameStateService::PlaySound(Sound sound, real duration)
{
    SoundEvent sevent(SoundEventCodes::Play, this, sound, duration);
    mySoundPin.Send(sevent);
}

void GameStateService::AnimationDone()
{
    PlayAnimation();
}

bool GameStateService::StateUpdate(Event &event, InPin pin)
{
    GameStateEvent &gevent = dynamic_cast<GameStateEvent&>(event);
    Update(gevent.GetState());
    return true;
}
