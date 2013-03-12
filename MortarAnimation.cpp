
#include "MortarAnimation.h"

const real MortarAnimationData::StartHeight = 1.0;

void MortarAnimationData::Update(real t)
{
    VectorF4 v = (myEnd - myStart)*t + myStart;
    real c = StartHeight;
    real b = 2*(myHeight - StartHeight + sqrt(myHeight*myHeight - myHeight * StartHeight));
    real a = -b-c;
    v[Y] = a*t*t+b*t+c;
    MatrixF4 m = Animated->Transform.Get();
    m.SetTranslation(v);
    Animated->Transform.Set(m);
}
