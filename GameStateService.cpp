#include "GameStateService.h"
#include "entity/Billboard.h"

#define XOR(p1, p2) ((p1 || p2) && !(p1 && p2))

void GameStateService::Player::Update(const PlayerState &other)
{
    if(Name != other.Name)
        throw Error(Error::InvalidValue, "Updating name of player");

    StatsDirty |= other.Health != Health || other.Score != Score || 
        XOR(other.Index == 0, Index == 0);
    StateDirty |= other.Position != Position;
    Index = other.Index;
    if(StatsDirty)
    {
        Health = other.Health;
        Score = other.Score;
    }
    if(StateDirty)
    {
        Position = other.Position;
        VectorF2 pos(
                Hexmap::jOffset[X]*Position[X]+Hexmap::kOffset[X]*Position[Y],
                Hexmap::jOffset[Y]*Position[X]+Hexmap::kOffset[Y]*Position[Y]);
        PlayerMovable->Transform.Set(
                MatrixF4::Translation(VectorF4(pos[X],0.0,pos[Y])));
    }
    StatsDirty = false;
    StateDirty = false;
}

GameStateService::GameStateService(MultiContainer *container, Hexmap *map,
        AssetRef<Texture> figureTexture) 
    : myAnimations(this), Turn(-1), myContainer(container), myMap(map), 
    myFigureTexture(figureTexture), myActionCount(0), myActionCursor(0)
{
    RegisterInPin(SkyportEventClass::GameState, "StateUpdates", 
            static_cast<EventCallback>(&GameStateService::StateUpdate));
    myDonePin = RegisterOutPin(SkyportEventClass::GameState, "Done");
    myTitle.Color.Set(ColorRGBA(255,255,0,255));
    myTitle.Text.Set("Welcome");
    myTitle.Anchors.Set(Anchor::Top);
    myTitle.Size.Set(SizeF2(0.1,0.2));
    myTitle.Position.Set(VectorF2(0,0.1));

    mySubtitle.Color.Set(ColorRGBA(255,255,0,255));
    mySubtitle.Text.Set("Game will begin shortly");
    mySubtitle.Anchors.Set(Anchor::Bottom);
    mySubtitle.Size.Set(SizeF2(0.1,0.15));

    container->AddChild(&myTitle);
    container->AddChild(&mySubtitle);
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

void GameStateService::SetCurrentPlayer()
{
    while(myCurrentPlayer->Index != 0)
    {
        myCurrentPlayer++;
        if(myCurrentPlayer == Players.end())
            myCurrentPlayer = Players.begin();
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
            Movable *nameMov = new Movable();
            MultiContainer *container = new MultiContainer();
            Nametag *nametag = new Nametag();
            bill->Offset.Set(VectorF2(0,0.5));
            nametag->Offset.Set(VectorF2(0,1.0));

            nametag->PlayerName.Set(pit->Name);
            nametag->Health.Set(pit->Health/100.0f);

            mov->SetChild(container);
            container->AddChild(bill);
            container->AddChild(nameMov);
            nameMov->SetChild(nametag);
            myContainer->AddChild(mov);
            bill->ProgramState().SetUniform("Z", -0.05f);
            bill->ProgramState().SetUniform("FrameCount", VectorI2(1,1));
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

        //myStats = new Statusbox();
        //myStats->State.Set(state);
        //myContainer->AddChild(myStats);
    }
    else
    {
        int i = 0;
        for(auto pit = state.Players_begin(); 
                pit != state.Players_end(); pit++)
        {
            Players[i++].Update(*pit);
        }
    }
    for(int j = 0;  j < mapSize[X]; j++)
    {
        for(int k = 0;  k < mapSize[Y]; k++)
        {
            myMap->SetTileType(j,k,state.GetMap()(j,k));
        }
    }
    SetCurrentPlayer();

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

VectorF2 DirectionToOffset(Direction dir)
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
    return Hexmap::jOffset * tileoff[X] + Hexmap::kOffset * tileoff[Y];
} 

void GameStateService::PlayAnimation()
{
    if(myAnimations.GetAnimationCount() == 1)
    {
        if(myActionCursor < myActionCount)
        {
            switch(myActionStates[myActionCursor].GetAction())
            {
                case SkyportAction::Move:
                    {
                        VectorF4 pos;
                        myCurrentPlayer->PlayerMovable->Transform.Get()
                            .GetTranslation(pos);

                        VectorF2 off = DirectionToOffset(
                                myActionStates[myActionCursor].GetDirection());
                        AnimationHelper::TranslationAnimationData *trdata =
                            new AnimationHelper::TranslationAnimationData(
                                myCurrentPlayer->PlayerMovable,
                                pos, 
                                VectorF4(pos[X] + off[X], pos[Y], pos[Z]+off[Y]), 
                                1, AnimationHelper::SmoothCurve);
                        myAnimations.AddAnimation(trdata);

                        //AnimationHelper::TextureAnimationData *tedata =
                        //    new AnimationHelper::TextureAnimationData(
                        //        myCurrentPlayer->PlayerVisual, 4, X, 1,
                        //        AnimationHelper::LinearCurve);
                        //myAnimations.AddAnimation(tedata);
                    }
                    break;
                default:
                    Debug("Action not implemented, may hang.");
            }
            myActionCursor++;
        }
        else
        {
            Event nevent(SkyportEventClass::GameState, 
                    GameStateEventCodes::StateProcessed, this);
            myDonePin.Send(nevent);
        }
    }
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
