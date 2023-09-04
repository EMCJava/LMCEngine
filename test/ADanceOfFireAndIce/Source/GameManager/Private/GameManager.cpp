//
// Created by samsa on 7/8/2023.
//

#include "GameManager.hpp"
#include "ToleranceBar.hpp"

#include <Engine/Engine.hpp>
#include <Engine/Core/UI/RectButton.hpp>
#include <Engine/Core/Environment/GlobalResourcePool.hpp>
#include <Engine/Core/Utilities/StopWatch.hpp>
#include <Engine/Core/Concept/ConceptCoreToImGuiImpl.hpp>
#include <Engine/Core/Graphic/API/GraphicAPI.hpp>
#include <Engine/Core/Graphic/Camera/PureConceptCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareAnimatedTexture.hpp>
#include <Engine/Core/Input/UserInput.hpp>

// To export symbol, used for runtime inspection
#include <Engine/Core/Concept/ConceptCoreRuntime.inl>

#include <spdlog/spdlog.h>

#include <glm/gtx/compatibility.hpp>

#pragma warning( Tempory for keycode )
#include <GLFW/glfw3.h>

DEFINE_CONCEPT_DS_MA_SE( GameManager )
DEFINE_SIMPLE_IMGUI_TYPE( GameManager, m_BPM, m_CameraLerp, m_TileSpriteSetRef, m_ToleranceBar )

namespace
{
std::vector<TileSpriteSet::TileMeta> TmpMap {
    { 180, 0 },
    { 180, 1193 },
    { 180, 1493 },
    { 180, 1793 },
    { 180, 2093 },
    { 180, 2393 },
    { 180, 2693 },
    { 180, 2993 },
    { 90, 3293 },
    { 90, 3443 },
    { 180, 3593 },
    { 180, 3893 },
    { 180, 4193 },
    { 180, 4493 },
    { 270, 4793, true },
    { 90, 4943, true },
    { 270, 5093, true },
    { 90, 5243, true },
    { 315, 5393, true },
    { 45, 5468, true },
    { 315, 5543, true },
    { 45, 5618, true },
    { 180, 5693 },
    { 270, 5993, true },
    { 90, 6143, true },
    { 270, 6293, true },
    { 90, 6443, true },
    { 270, 6593, true },
    { 45, 6743, true },
    { 315, 6818, true },
    { 90, 6893, true },
    { 270, 7043, true },
    { 90, 7193, true },
    { 270, 7343, true },
    { 90, 7493, true },
    { 270, 7643, true },
    { 45, 7793, true },
    { 45, 7868 },
    { 45, 7943 },
    { 45, 8018 },
    { 90, 8093 },
    { 90, 8243 },
    { 90, 8393 },
    { 90, 8543 },
    { 90, 8693 },
    { 45, 8843 },
    { 45, 8918 },
    { 90, 8993 },
    { 90, 9143 },
    { 45, 9293 },
    { 45, 9368 },
    { 90, 9443 },
    { 180, 9593 },
    { 180, 9893 },
    { 180, 10193 },
    { 45, 10493 },
    { 135, 10568 },
    { 180, 10793 },
    { 90, 11093 },
    { 90, 11243 },
    { 90, 11393 },
    { 90, 11543 },
    { 90, 11693 },
    { 180, 11843 },
    { 90, 12143 },
    { 180, 12293 },
    { 90, 12593 },
    { 90, 12743 },
    { 90, 12893 },
    { 90, 13043 },
    { 180, 13193 },
    { 90, 13493 },
    { 90, 13643 },
    { 90, 13793 },
    { 90, 13943 },
    { 90, 14093 },
    { 180, 14243 },
    { 90, 14543 },
    { 180, 14693 },
    { 90, 14993 },
    { 90, 15143 },
    { 90, 15293 },
    { 90, 15443 },
    { 180, 15593 },
    { 90, 15893 },
    { 90, 16043 },
    { 90, 16193 },
    { 90, 16343 },
    { 90, 16493 },
    { 180, 16643 },
    { 90, 16943 },
    { 90, 17093 },
    { 90, 17243 },
    { 90, 17393 },
    { 90, 17543 },
    { 90, 17693 },
    { 180, 17843 },
    { 90, 18143 },
    { 90, 18293 },
    { 90, 18443 },
    { 90, 18593 },
    { 90, 18743 },
    { 90, 18893 },
    { 180, 19043 },
    { 90, 19343 },
    { 90, 19493 },
    { 90, 19643 },
    { 90, 19793 },
    { 90, 19943 },
    { 90, 20093 },
    { 90, 20243 },
    { 90, 20393 },
    { 45, 20543 },
    { 45, 20618 },
    { 90, 20693 },
    { 90, 20843 },
    { 45, 20993 },
    { 45, 21068 },
    { 90, 21143 },
    { 90, 21293 },
    { 90, 21443 },
    { 90, 21593 },
    { 45, 21743 },
    { 45, 21818 },
    { 90, 21893 },
    { 90, 22043 },
    { 45, 22193 },
    { 45, 22268 },
    { 45, 22343 },
    { 45, 22418 },
    { 90, 22493 },
    { 90, 22643 },
    { 90, 22793 },
    { 45, 22943 },
    { 45, 23018 },
    { 90, 23093 },
    { 90, 23243 },
    { 45, 23393 },
    { 45, 23468 },
    { 45, 23543 },
    { 45, 23618 },
    { 90, 23693 },
    { 90, 23843 },
    { 90, 23993 },
    { 45, 24143 },
    { 45, 24218 },
    { 90, 24293 },
    { 90, 24443 },
    { 45, 24593 },
    { 45, 24668 },
    { 90, 24743 },
    { 45, 24893 },
    { 45, 24968 },
    { 90, 25043 },
    { 90, 25193 },
    { 45, 25343 },
    { 45, 25418 },
    { 90, 25493 },
    { 90, 25643 },
    { 45, 25793 },
    { 45, 25868 },
    { 45, 25943 },
    { 45, 26018 },
    { 90, 26093 },
    { 90, 26243 },
    { 45, 26393 },
    { 45, 26468 },
    { 90, 26543 },
    { 90, 26693 },
    { 45, 26843 },
    { 45, 26918 },
    { 45, 26993 },
    { 45, 27068 },
    { 90, 27143 },
    { 90, 27293 },
    { 90, 27443 },
    { 90, 27593 },
    { 45, 27743 },
    { 45, 27818 },
    { 90, 27893 },
    { 90, 28043 },
    { 45, 28193 },
    { 45, 28268 },
    { 90, 28343 },
    { 90, 28493 },
    { 90, 28643 },
    { 45, 28793 },
    { 45, 28868 },
    { 45, 28943 },
    { 45, 29018 },
    { 90, 29093 },
    { 90, 29243 },
    { 180, 29393 },
    { 90, 29693 },
    { 90, 29843 },
    { 90, 29993 },
    { 45, 30143 },
    { 45, 30218 },
    { 90, 30293 },
    { 90, 30443 },
    { 135, 30593 },
    { 45, 30818 },
    { 90, 30893 },
    { 90, 31043 },
    { 90, 31193 },
    { 45, 31343 },
    { 45, 31418 },
    { 90, 31493 },
    { 90, 31643 },
    { 90, 31793 },
    { 45, 31943 },
    { 45, 32018 },
    { 90, 32093 },
    { 90, 32243 },
    { 90, 32393 },
    { 45, 32543 },
    { 45, 32618 },
    { 90, 32693 },
    { 90, 32843 },
    { 135, 32993 },
    { 45, 33218 },
    { 90, 33293 },
    { 90, 33443 },
    { 90, 33593 },
    { 45, 33743 },
    { 45, 33818 },
    { 90, 33893 },
    { 90, 34043 },
    { 45, 34193 },
    { 45, 34268 },
    { 45, 34343 },
    { 45, 34418 },
    { 90, 34493 },
    { 90, 34643 },
    { 90, 34793 },
    { 45, 34943 },
    { 45, 35018 },
    { 90, 35093 },
    { 90, 35243 },
    { 45, 35393 },
    { 45, 35468 },
    { 45, 35543 },
    { 45, 35618 },
    { 90, 35693 },
    { 90, 35843 },
    { 45, 35993 },
    { 45, 36068 },
    { 45, 36143 },
    { 45, 36218 },
    { 45, 36293 },
    { 45, 36368 },
    { 90, 36443 },
    { 45, 36593 },
    { 45, 36668 },
    { 45, 36743 },
    { 45, 36818 },
    { 45, 36893 },
    { 45, 36968 },
    { 90, 37043 },
    { 90, 37193 },
    { 45, 37343 },
    { 45, 37418 },
    { 45, 37493 },
    { 45, 37568 },
    { 45, 37643 },
    { 45, 37718 },
    { 45, 37793 },
    { 45, 37868 },
    { 45, 37943 },
    { 45, 38018 },
    { 45, 38093 },
    { 45, 38168 },
    { 45, 38243 },
    { 45, 38318 },
    { 90, 38393 },
    { 90, 38543 },
    { 90, 38693 },
    { 90, 38843 },
    { 180, 38993 },
    { 90, 39293 },
    { 90, 39443 },
    { 180, 39593 },
    { 180, 39893 },
    { 180, 40193 },
    { 180, 40493 },
    { 180, 40793 },
    { 180, 41093 },
    { 180, 41393 },
    { 90, 41693 },
    { 90, 41843 },
    { 180, 41993 },
    { 180, 42293 },
    { 180, 42593 },
    { 180, 42893 },
    { 90, 43193 },
    { 45, 43343 },
    { 45, 43418 },
    { 90, 43493 },
    { 90, 43643 },
    { 45, 43793 },
    { 45, 43868 },
    { 45, 43943 },
    { 45, 44018 },
    { 90, 44093 },
    { 90, 44243 },
    { 180, 44393 },
    { 180, 44693 },
    { 180, 44993 },
    { 180, 45293 },
    { 180, 45593 },
    { 180, 45893 },
    { 180, 46193 },
    { 90, 46493 },
    { 90, 46643 },
    { 180, 46793 },
    { 180, 47093 },
    { 180, 47393 },
    { 180, 47693 },
    { 90, 47993 },
    { 45, 48143 },
    { 45, 48218 },
    { 90, 48293 },
    { 90, 48443 },
    { 45, 48593 },
    { 45, 48668 },
    { 45, 48743 },
    { 45, 48818 },
    { 45, 48893 },
    { 45, 48968 },
    { 90, 49043 },
    { 135, 49193 },
    { 45, 49418 },
    { 90, 49493 },
    { 90, 49643 },
    { 45, 49793 },
    { 90, 49868 },
    { 45, 50018 },
    { 90, 50093 },
    { 90, 50243 },
    { 45, 50393 },
    { 45, 50468 },
    { 90, 50543 },
    { 45, 50693 },
    { 45, 50768 },
    { 90, 50843 },
    { 45, 50993 },
    { 45, 51068 },
    { 45, 51143 },
    { 45, 51218 },
    { 90, 51293 },
    { 90, 51443 },
    { 90, 51593 },
    { 45, 51743 },
    { 45, 51818 },
    { 90, 51893 },
    { 90, 52043 },
    { 45, 52193 },
    { 45, 52268 },
    { 45, 52343 },
    { 45, 52418 },
    { 90, 52493 },
    { 90, 52643 },
    { 90, 52793 },
    { 45, 52943 },
    { 45, 53018 },
    { 90, 53093 },
    { 90, 53243 },
    { 45, 53393 },
    { 45, 53468 },
    { 45, 53543 },
    { 45, 53618 },
    { 45, 53693 },
    { 45, 53768 },
    { 90, 53843 },
    { 135, 53993 },
    { 45, 54218 },
    { 90, 54293 },
    { 90, 54443 },
    { 45, 54593 },
    { 90, 54668 },
    { 45, 54818 },
    { 90, 54893 },
    { 90, 55043 },
    { 45, 55193 },
    { 45, 55268 },
    { 90, 55343 },
    { 45, 55493 },
    { 45, 55568 },
    { 90, 55643 },
    { 60, 55793 },
    { 60, 55893 },
    { 60, 55993 },
    { 60, 56093 },
    { 60, 56193 },
    { 60, 56293 },
    { 90, 56393 },
    { 45, 56543 },
    { 45, 56618 },
    { 90, 56693 },
    { 90, 56843 },
    { 45, 56993 },
    { 45, 57068 },
    { 45, 57143 },
    { 45, 57218 },
    { 90, 57293 },
    { 90, 57443 },
    { 90, 57593 },
    { 45, 57743 },
    { 45, 57818 },
    { 90, 57893 },
    { 90, 58043 },
    { 45, 58193 },
    { 45, 58268 },
    { 90, 58343 },
    { 30, 58493 },
    { 30, 58543 },
    { 30, 58593 },
    { 90, 58643 },
    { 180, 58793 },
    { 90, 59093 },
    { 90, 59243 },
    { 90, 59393 },
    { 45, 59543 },
    { 45, 59618 },
    { 90, 59693 },
    { 90, 59843 },
    { 90, 59993 },
    { 45, 60143 },
    { 45, 60218 },
    { 90, 60293 },
    { 90, 60443 },
    { 45, 60593 },
    { 45, 60668 },
    { 45, 60743 },
    { 45, 60818 },
    { 90, 60893 },
    { 90, 61043 },
    { 90, 61193 },
    { 90, 61343 },
    { 90, 61493 },
    { 45, 61643 },
    { 45, 61718 },
    { 90, 61793 },
    { 45, 61943 },
    { 45, 62018 },
    { 45, 62093 },
    { 45, 62168 },
    { 90, 62243 },
    { 180, 62393 },
    { 60, 62693 },
    { 60, 62793 },
    { 60, 62893 },
    { 60, 62993 },
    { 60, 63093 },
    { 60, 63193 },
    { 60, 63293 },
    { 60, 63393 },
    { 60, 63493 },
    { 120, 63593 },
    { 60, 63793 },
    { 120, 63893 },
    { 60, 64093 },
    { 120, 64193 },
    { 60, 64393 },
    { 120, 64493 },
    { 60, 64693 },
    { 120, 64793 },
    { 60, 64993 },
    { 60, 65093 },
    { 60, 65193 },
    { 60, 65293 },
    { 120, 65393 },
    { 60, 65593 },
    { 60, 65693 },
    { 60, 65793 },
    { 60, 65893 },
    { 120, 65993 },
    { 60, 66193 },
    { 120, 66293 },
    { 60, 66493 },
    { 120, 66593 },
    { 60, 66793 },
    { 120, 66893 },
    { 60, 67093 },
    { 120, 67193 },
    { 60, 67393 },
    { 60, 67493 },
    { 60, 67593 },
    { 60, 67693 },
    { 120, 67793 },
    { 60, 67993 },
    { 180, 68093 },
    { 120, 68393 },
    { 60, 68593 },
    { 120, 68693 },
    { 60, 68893 },
    { 120, 68993 },
    { 60, 69193 },
    { 120, 69293 },
    { 60, 69493 },
    { 120, 69593 },
    { 60, 69793 },
    { 60, 69893 },
    { 60, 69993 },
    { 60, 70093 },
    { 120, 70193 },
    { 60, 70393 },
    { 60, 70493 },
    { 60, 70593 },
    { 60, 70693 },
    { 120, 70793 },
    { 60, 70993 },
    { 120, 71093 },
    { 60, 71293 },
    { 120, 71393 },
    { 60, 71593 },
    { 120, 71693 },
    { 60, 71893 },
    { 120, 71993 },
    { 60, 72193 },
    { 60, 72293 },
    { 60, 72393 },
    { 60, 72493 },
    { 90, 72593 },
    { 180, 72743 },
    { 180, 73043 },
    { 45, 73343 },
    { 45, 73418 },
    { 90, 73493 },
    { 90, 73643 },
    { 45, 73793 },
    { 45, 73868 },
    { 90, 73943 },
    { 90, 74093 },
    { 90, 74243 },
    { 90, 74393 },
    { 45, 74543 },
    { 45, 74618 },
    { 90, 74693 },
    { 90, 74843 },
    { 45, 74993 },
    { 45, 75068 },
    { 45, 75143 },
    { 45, 75218 },
    { 90, 75293 },
    { 180, 75443 },
    { 45, 75743 },
    { 45, 75818 },
    { 90, 75893 },
    { 90, 76043 },
    { 45, 76193 },
    { 45, 76268 },
    { 45, 76343 },
    { 45, 76418 },
    { 90, 76493 },
    { 90, 76643 },
    { 90, 76793 },
    { 45, 76943 },
    { 45, 77018 },
    { 90, 77093 },
    { 90, 77243 },
    { 45, 77393 },
    { 45, 77468 },
    { 45, 77543 },
    { 45, 77618 },
    { 90, 77693 },
    { 180, 77843 },
    { 45, 78143 },
    { 45, 78218 },
    { 90, 78293 },
    { 90, 78443 },
    { 45, 78593 },
    { 45, 78668 },
    { 45, 78743 },
    { 45, 78818 },
    { 90, 78893 },
    { 90, 79043 },
    { 45, 79193 },
    { 45, 79268 },
    { 90, 79343 },
    { 90, 79493 },
    { 45, 79643 },
    { 45, 79718 },
    { 45, 79793 },
    { 45, 79868 },
    { 90, 79943 },
    { 90, 80093 },
    { 90, 80243 },
    { 90, 80393 },
    { 45, 80543 },
    { 45, 80618 },
    { 90, 80693 },
    { 90, 80843 },
    { 45, 80993 },
    { 45, 81068 },
    { 90, 81143 },
    { 90, 81293 },
    { 45, 81443 },
    { 45, 81518 },
    { 45, 81593 },
    { 45, 81668 },
    { 45, 81743 },
    { 45, 81818 },
    { 90, 81893 },
    { 90, 82043 },
    { 180, 82193 },
    { 90, 82493 },
    { 90, 82643 },
    { 135, 82793 },
    { 45, 83018 },
    { 90, 83093 },
    { 45, 83243 },
    { 45, 83318 },
    { 135, 83393 },
    { 45, 83618 },
    { 90, 83693 },
    { 90, 83843 },
    { 90, 83993 },
    { 45, 84143 },
    { 45, 84218 },
    { 90, 84293 },
    { 90, 84443 },
    { 90, 84593 },
    { 45, 84743 },
    { 90, 84818 },
    { 45, 84968 },
    { 90, 85043 },
    { 135, 85193 },
    { 45, 85418 },
    { 90, 85493 },
    { 45, 85643 },
    { 45, 85718 },
    { 135, 85793 },
    { 45, 86018 },
    { 90, 86093 },
    { 90, 86243 },
    { 90, 86393 },
    { 45, 86543 },
    { 45, 86618 },
    { 90, 86693 },
    { 90, 86843 },
    { 45, 86993 },
    { 45, 87068 },
    { 45, 87143 },
    { 45, 87218 },
    { 90, 87293 },
    { 90, 87443 },
    { 45, 87593 },
    { 45, 87668 },
    { 45, 87743 },
    { 45, 87818 },
    { 90, 87893 },
    { 90, 88043 },
    { 45, 88193 },
    { 45, 88268 },
    { 45, 88343 },
    { 45, 88418 },
    { 90, 88493 },
    { 90, 88643 },
    { 45, 88793 },
    { 45, 88868 },
    { 45, 88943 },
    { 45, 89018 },
    { 90, 89093 },
    { 45, 89243 },
    { 45, 89318 },
    { 45, 89393 },
    { 45, 89468 },
    { 45, 89543 },
    { 45, 89618 },
    { 45, 89693 },
    { 45, 89768 },
    { 90, 89843 },
    { 45, 89993 },
    { 45, 90068 },
    { 45, 90143 },
    { 45, 90218 },
    { 45, 90293 },
    { 45, 90368 },
    { 45, 90443 },
    { 45, 90518 },
    { 45, 90593 },
    { 45, 90668 },
    { 45, 90743 },
    { 45, 90818 },
    { 45, 90893 },
    { 45, 90968 },
    { 45, 91043 },
    { 45, 91118 },
    { 120, 91193 },
    { 60, 91393 },
    { 120, 91493 },
    { 60, 91693 },
    { 120, 91793 },
    { 60, 91993 },
    { 120, 92093 },
    { 60, 92293 },
    { 180, 92393 },
    { 180, 92693 },
    { 60, 92993 },
    { 60, 93093 },
    { 60, 93193 },
    { 180, 93293 },
    { 180, 93593 },
    { 180, 93893 },
    { 180, 94193 },
    { 180, 94493 },
    { 180, 94793 },
    { 180, 95093 },
    { 180, 95393 },
    { 90, 95693 },
    { 90, 95843 },
    { 180, 95993 },
    { 180, 96293 },
    { 180, 96593 },
    { 180, 96893 },
    { 90, 97193 },
    { 45, 97343 },
    { 45, 97418 },
    { 90, 97493 },
    { 90, 97643 },
    { 45, 97793 },
    { 45, 97868 },
    { 45, 97943 },
    { 45, 98018 },
    { 90, 98093 },
    { 90, 98243 },
    { 180, 98393 },
    { 180, 98693 },
    { 180, 98993 },
    { 180, 99293 },
    { 180, 99593 },
    { 180, 99893 },
    { 180, 100193 },
    { 90, 100493 },
    { 90, 100643 },
    { 180, 100793 },
    { 180, 101093 },
    { 180, 101393 },
    { 180, 101693 },
    { 360, 101993 },
    { 45, 102593 },
    { 45, 102668 },
    { 45, 102743 },
    { 45, 102818 },
    { 45, 102893 },
    { 45, 102968 },
    { 90, 103043 },
    { 135, 103193 },
    { 45, 103418 },
    { 90, 103493 },
    { 90, 103643 },
    { 45, 103793 },
    { 90, 103868 },
    { 45, 104018 },
    { 90, 104093 },
    { 90, 104243 },
    { 45, 104393 },
    { 45, 104468 },
    { 90, 104543 },
    { 45, 104693 },
    { 45, 104768 },
    { 90, 104843 },
    { 45, 104993 },
    { 45, 105068 },
    { 45, 105143 },
    { 45, 105218 },
    { 90, 105293 },
    { 90, 105443 },
    { 90, 105593 },
    { 45, 105743 },
    { 45, 105818 },
    { 90, 105893 },
    { 90, 106043 },
    { 45, 106193 },
    { 45, 106268 },
    { 45, 106343 },
    { 45, 106418 },
    { 90, 106493 },
    { 90, 106643 },
    { 90, 106793 },
    { 45, 106943 },
    { 45, 107018 },
    { 90, 107093 },
    { 90, 107243 },
    { 45, 107393 },
    { 45, 107468 },
    { 45, 107543 },
    { 45, 107618 },
    { 45, 107693 },
    { 45, 107768 },
    { 90, 107843 },
    { 135, 107993 },
    { 45, 108218 },
    { 90, 108293 },
    { 90, 108443 },
    { 45, 108593 },
    { 90, 108668 },
    { 45, 108818 },
    { 90, 108893 },
    { 90, 109043 },
    { 45, 109193 },
    { 45, 109268 },
    { 90, 109343 },
    { 45, 109493 },
    { 45, 109568 },
    { 90, 109643 },
    { 60, 109793 },
    { 60, 109893 },
    { 60, 109993 },
    { 60, 110093 },
    { 60, 110193 },
    { 60, 110293 },
    { 90, 110393 },
    { 45, 110543 },
    { 45, 110618 },
    { 90, 110693 },
    { 90, 110843 },
    { 45, 110993 },
    { 45, 111068 },
    { 45, 111143 },
    { 45, 111218 },
    { 90, 111293 },
    { 90, 111443 },
    { 90, 111593 },
    { 45, 111743 },
    { 45, 111818 },
    { 90, 111893 },
    { 90, 112043 },
    { 45, 112193 },
    { 45, 112268 },
    { 90, 112343 },
    { 30, 112493 },
    { 30, 112543 },
    { 30, 112593 },
    { 90, 112643 },
    { 180, 112793 },
    { 90, 113093 },
    { 90, 113243 },
    { 90, 113393 },
    { 45, 113543 },
    { 45, 113618 },
    { 90, 113693 },
    { 90, 113843 },
    { 90, 113993 },
    { 45, 114143 },
    { 45, 114218 },
    { 90, 114293 },
    { 90, 114443 },
    { 45, 114593 },
    { 45, 114668 },
    { 45, 114743 },
    { 45, 114818 },
    { 90, 114893 },
    { 90, 115043 },
    { 180, 115193 },
    { 90, 115493 },
    { 45, 115643 },
    { 45, 115718 },
    { 90, 115793 },
    { 45, 115943 },
    { 45, 116018 },
    { 45, 116093 },
    { 45, 116168 },
    { 90, 116243 },
    { 90, 116393 },
    { 90, 116543 },
    { 45, 116693 },
    { 45, 116768 },
    { 90, 116843 },
    { 60, 116993 },
    { 60, 117093 },
    { 60, 117193 },
    { 60, 117293 },
    { 60, 117393 },
    { 60, 117493 },
    { 90, 117593 },
    { 45, 117743 },
    { 45, 117818 },
    { 90, 117893 },
    { 45, 118043 },
    { 45, 118118 },
    { 45, 118193 },
    { 45, 118268 },
    { 45, 118343 },
    { 45, 118418 },
    { 90, 118493 },
    { 90, 118643 },
    { 45, 118793 },
    { 45, 118868 },
    { 45, 118943 },
    { 45, 119018 },
    { 90, 119093 },
    { 45, 119243 },
    { 45, 119318 },
    { 45, 119393 },
    { 45, 119468 },
    { 45, 119543 },
    { 45, 119618 },
    { 45, 119693 },
    { 45, 119768 },
    { 90, 119843 },
    { 45, 119993 },
    { 45, 120068 },
    { 45, 120143 },
    { 45, 120218 },
    { 45, 120293 },
    { 45, 120368 },
    { 45, 120443 },
    { 45, 120518 },
    { 45, 120593 },
    { 45, 120668 },
    { 45, 120743 },
    { 45, 120818 },
    { 45, 120893 },
    { 45, 120968 },
    { 45, 121043 },
    { 45, 121118 },
    { 360, 121193 },
    { 360, 121793, false, 200 / 3.F },
    { 360, 123593, false, 200 },
    { 360, 124193 },
    { 360, 124793 },
    { 360, 125393 },
    { 360, 125993 },
    { 360, 126593 },
    { 180, 127193 },
    { 180, 127493 },
    { 180, 127793 },
    { 180, 128093 },
    { 360, 128393 },
    { 360, 128993 },
    { 360, 129593 },
    { 360, 130193 },
    { 180, 130793 },
    { 180, 131093 },
    { 180, 131393 },
    { 180, 131693 },
    { 360, 131993 },
    { 360, 132593 },
    { 90, 133193 },
    { 90, 133343 },
    { 90, 133493 },
    { 90, 133643 },
    { 90, 133793 },
    { 90, 133943 },
    { 90, 134093 },
    { 90, 134243 },
    { 90, 134393 },
    { 90, 134543 },
    { 90, 134693 },
    { 90, 134843 },
    { 45, 134993 },
    { 45, 135068 },
    { 45, 135143 },
    { 45, 135218 },
    { 180, 135293 },
    { 135, 135593 },
    { 45, 135818 },
    { 90, 135893 },
    { 90, 136043 },
    { 45, 136193 },
    { 90, 136268 },
    { 45, 136418 },
    { 90, 136493 },
    { 90, 136643 },
    { 45, 136793 },
    { 45, 136868 },
    { 90, 136943 },
    { 45, 137093 },
    { 45, 137168 },
    { 90, 137243 },
    { 45, 137393 },
    { 45, 137468 },
    { 45, 137543 },
    { 45, 137618 },
    { 90, 137693 },
    { 90, 137843 },
    { 90, 137993 },
    { 45, 138143 },
    { 45, 138218 },
    { 90, 138293 },
    { 90, 138443 },
    { 45, 138593 },
    { 45, 138668 },
    { 45, 138743 },
    { 45, 138818 },
    { 90, 138893 },
    { 90, 139043 },
    { 90, 139193 },
    { 45, 139343 },
    { 45, 139418 },
    { 90, 139493 },
    { 90, 139643 },
    { 45, 139793 },
    { 45, 139868 },
    { 45, 139943 },
    { 45, 140018 },
    { 45, 140093 },
    { 45, 140168 },
    { 90, 140243 },
    { 135, 140393 },
    { 45, 140618 },
    { 90, 140693 },
    { 90, 140843 },
    { 45, 140993 },
    { 90, 141068 },
    { 45, 141218 },
    { 90, 141293 },
    { 90, 141443 },
    { 45, 141593 },
    { 45, 141668 },
    { 90, 141743 },
    { 45, 141893 },
    { 45, 141968 },
    { 90, 142043 },
    { 45, 142193 },
    { 45, 142268 },
    { 45, 142343 },
    { 45, 142418 },
    { 45, 142493 },
    { 45, 142568 },
    { 45, 142643 },
    { 45, 142718 },
    { 90, 142793 },
    { 45, 142943 },
    { 45, 143018 },
    { 90, 143093 },
    { 90, 143243 },
    { 45, 143393 },
    { 45, 143468 },
    { 45, 143543 },
    { 45, 143618 },
    { 90, 143693 },
    { 90, 143843 },
    { 90, 143993 },
    { 45, 144143 },
    { 45, 144218 },
    { 90, 144293 },
    { 90, 144443 },
    { 45, 144593 },
    { 45, 144668 },
    { 45, 144743 },
    { 45, 144818 },
    { 30, 144893 },
    { 30, 144943 },
    { 30, 144993 },
    { 90, 145043 },
    { 180, 145193 },
    { 180, 145493 },
    { 180, 145793 },
    { 180, 146093 },
    { 180, 146393 },
    { 180, 146693 },
    { 180, 146993 },
    { 90, 147293 },
    { 90, 147443 },
    { 180, 147593 },
    { 180, 147893 },
    { 180, 148193 },
    { 180, 148493 },
    { 90, 148793 },
    { 90, 148943 },
    { 90, 149093 },
    { 90, 149243 },
    { 90, 149393 },
    { 90, 149543 },
    { 90, 149693 },
    { 90, 149843 },
    { 135, 149993 },
    { 45, 150218 },
    { 90, 150293 },
    { 90, 150443 },
    { 135, 150593 },
    { 45, 150818 },
    { 90, 150893 },
    { 90, 151043 },
    { 135, 151193 },
    { 45, 151418 },
    { 90, 151493 },
    { 90, 151643 },
    { 135, 151793 },
    { 45, 152018 },
    { 90, 152093 },
    { 90, 152243 },
    { 135, 152393 },
    { 45, 152618 },
    { 90, 152693 },
    { 90, 152843 },
    { 90, 152993 },
    { 90, 153143 },
    { 90, 153293 },
    { 90, 153443 },
    { 180, 153593 },
    { 45, 153893 },
    { 45, 153968 },
    { 90, 154043 },
    { 45, 154193 },
    { 45, 154268 },
    { 45, 154343 },
    { 45, 154418 },
    { 45, 154493 },
    { 45, 154568 },
    { 90, 154643 },
    { 135, 154793 },
    { 45, 155018 },
    { 90, 155093 },
    { 90, 155243 },
    { 45, 155393 },
    { 45, 155468 },
    { 45, 155543 },
    { 45, 155618 },
    { 90, 155693 },
    { 90, 155843 },
    { 45, 155993 },
    { 45, 156068 },
    { 45, 156143 },
    { 45, 156218 },
    { 45, 156293 },
    { 45, 156368 },
    { 90, 156443 },
    { 45, 156593 },
    { 45, 156668 },
    { 45, 156743 },
    { 45, 156818 },
    { 45, 156893 },
    { 45, 156968 },
    { 45, 157043 },
    { 45, 157118 },
    { 90, 157193 },
    { 45, 157343 },
    { 45, 157418 },
    { 90, 157493 },
    { 90, 157643 },
    { 45, 157793 },
    { 45, 157868 },
    { 45, 157943 },
    { 45, 158018 },
    { 90, 158093 },
    { 90, 158243 },
    { 45, 158393 },
    { 45, 158468 },
    { 90, 158543 },
    { 45, 158693 },
    { 45, 158768 },
    { 90, 158843 },
    { 45, 158993 },
    { 45, 159068 },
    { 45, 159143 },
    { 45, 159218 },
    { 45, 159293 },
    { 45, 159368 },
    { 90, 159443 },
    { 90, 159593 },
    { 45, 159743 },
    { 45, 159818 },
    { 90, 159893 },
    { 45, 160043 },
    { 45, 160118 },
    { 45, 160193 },
    { 45, 160268 },
    { 45, 160343 },
    { 45, 160418 },
    { 90, 160493 },
    { 90, 160643 },
    { 45, 160793 },
    { 45, 160868 },
    { 45, 160943 },
    { 45, 161018 },
    { 30, 161093 },
    { 30, 161143 },
    { 30, 161193 },
    { 90, 161243 },
    { 60, 161393 },
    { 60, 161493 },
    { 60, 161593 },
    { 60, 161693 },
    { 60, 161793 },
    { 60, 161893 },
    { 90, 161993 },
    { 45, 162143 },
    { 45, 162218 },
    { 45, 162293 },
    { 45, 162368 },
    { 90, 162443 },
    { 45, 162593 },
    { 45, 162668 },
    { 45, 162743 },
    { 45, 162818 },
    { 45, 162893 },
    { 45, 162968 },
    { 90, 163043 },
    { 90, 163193 },
    { 45, 163343 },
    { 45, 163418 },
    { 90, 163493 },
    { 45, 163643 },
    { 45, 163718 },
    { 45, 163793 },
    { 45, 163868 },
    { 45, 163943 },
    { 45, 164018 },
    { 30, 164093 },
    { 30, 164143 },
    { 30, 164193 },
    { 90, 164243 },
    { 180, 164393 },
    { 90, 164693 },
    { 90, 164843 },
    { 90, 164993 },
    { 45, 165143 },
    { 45, 165218 },
    { 90, 165293 },
    { 90, 165443 },
    { 90, 165593 },
    { 90, 165743 },
    { 90, 165893 },
    { 90, 166043 },
    { 45, 166193 },
    { 45, 166268 },
    { 45, 166343 },
    { 45, 166418 },
    { 90, 166493 },
    { 90, 166643 },
    { 90, 166793 },
    { 90, 166943 },
    { 90, 167093 },
    { 45, 167243 },
    { 45, 167318 },
    { 90, 167393 },
    { 90, 167543 },
    { 45, 167693 },
    { 45, 167768 },
    { 90, 167843 },
    { 90, 167993 },
    { 90, 168143 },
    { 90, 168293 },
    { 90, 168443 },
    { 45, 168593 },
    { 45, 168668 },
    { 90, 168743 },
    { 90, 168893 },
    { 90, 169043 },
    { 180, 169193 },
    { 90, 169493 },
    { 90, 169643 },
    { 90, 169793 },
    { 90, 169943 },
    { 180, 170093 },
    { 90, 170393 },
    { 90, 170543 },
    { 90, 170693 },
    { 90, 170843 },
    { 90, 170993 },
    { 90, 171143 },
    { 180, 171293 },
    { 90, 171593 },
    { 90, 171743 },
    { 90, 171893 },
    { 45, 172043 },
    { 45, 172118 },
    { 90, 172193 },
    { 90, 172343 },
    { 45, 172493 },
    { 45, 172568 },
    { 90, 172643 },
    { 180, 172793 },
    { 180, 173093 },
    { 180, 173393 },
    { 45, 173693 },
    { 360, 173768 }
};

}

const char* vertexTextureShaderSource   = "#version 330 core\n"
                                          "layout (location = 0) in vec3 aPos;\n"
                                          "layout (location = 1) in vec2 aTexCoord;\n"
                                          "out vec2 TexCoord;\n"
                                          "uniform mat4 projectionMatrix;\n"
                                          "uniform mat4 modelMatrix;\n"
                                          "void main()\n"
                                          "{\n"
                                          "   gl_Position = projectionMatrix * modelMatrix * vec4(aPos, 1.0);\n"
                                          "   TexCoord = aTexCoord;\n"
                                          "}\0";
const char* fragmentTextureShaderSource = "#version 330 core\n"
                                          "out vec4 FragColor;\n"
                                          "in vec2 TexCoord;\n"
                                          "uniform sampler2D sample_texture;\n"
                                          "void main()\n"
                                          "{\n"
                                          "   vec4 texColor = texture(sample_texture, TexCoord);\n"
                                          "   FragColor = texColor;\n"
                                          "}\n\0";

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    SetupCamera( );
    LoadAudio( );

    m_TileSpriteSetRef = AddConcept<TileSpriteSet>( );
    m_TileSpriteSetRef.lock( )->SetActiveCamera( m_Camera.get( ) );

    SetupShader( );

    LoadPlayerSprites( );

    LoadTileSprites( { 360, 315, 270, 240, 180, 135, 120, 90, 60, 45, 30 } );
    LoadTileMap( );

    SetupExplosionSpriteTemplate( );
    LoadToleranceSprite( );

    const auto& Button = AddConcept<RectButton>( 100, 100 );
    Button->SetActiveCamera( m_Camera.get( ) );
    Button->SetPressReactColor( glm::vec4 { 1, 1, 0.5, 1 } );

    m_InActivePlayerSprite->SetRotation( 0, 0, glm::radians( 180.f ) );

    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    const auto DeltaSecond    = Engine::GetEngine( )->GetDeltaSecond( );
    const bool PlayerInteract = IsUserPrimaryInteract( );

    if ( m_IsCheckingDeviceDelay ) [[unlikely]]
    {
        // Adjust the audio offset to the correct value
        (void) m_DelayCheckingHandle.GetCorrectedCurrentAudioOffset( );

        if ( PlayerInteract )
        {
            UpdateDeviceOffset( );
        }

        // End of offset wizard
        if ( m_DelayCheckingHandle.IsAudioEnded( ) )
        {
            spdlog::info( "End of offset wizard, final offset: {}ms", m_UserDeviceOffsetMS );
            m_IsCheckingDeviceDelay = false;

            m_MainAudioHandle.Resume( );
        }
    } else
    {
        auto TileSpriteSetShared = m_TileSpriteSetRef.lock( );

        const int64_t PlayPosition         = m_MainAudioHandle.GetCorrectedCurrentAudioOffset( ) - m_UserDeviceOffsetMS;
        const FloatTy NextPlayTilePosition = TileSpriteSetShared->GetNextStartTime( );
        const FloatTy DeltaTimeToNext      = NextPlayTilePosition - PlayPosition;

        if ( m_WaitingForFirstBeat )
        {
            if ( DeltaTimeToNext <= m_MSPB )
            {
                m_WaitingForFirstBeat = false;
            }
        } else if ( !TileSpriteSetShared->TileReachedEnd( ) )
        {
            /*
             *
             * Rotate player
             *
             * */

            const FloatTy  MinutePerSecond = 1 / 60.F;
            const FloatTy  BeatPerMinute   = m_BPM;
            constexpr auto RadiansPerBeat  = glm::radians<FloatTy>( 180 );

            int DirectionVector = m_PlayerDirectionClockWise ? -1 : 1;

            const auto RadiansPerSecond = RadiansPerBeat * BeatPerMinute * MinutePerSecond;
            m_InActivePlayerSprite->AlterRotation( 0, 0, DirectionVector * DeltaSecond * RadiansPerSecond );

            /*
             *
             * Camera move
             *
             * */
            if ( m_CameraLerp < 1 )
            {
                m_CameraLerp += DeltaSecond * 15;
                if ( m_CameraLerp >= 1 )
                {
                    m_Camera->SetCoordinate( m_CameraEnd.x, m_CameraEnd.y );
                    m_Camera->UpdateProjectionMatrix( );

                    m_CameraLerp = 1;
                } else
                {
                    const auto LerpCoordinate = glm::lerp( m_CameraStart, m_CameraEnd, m_CameraLerp );
                    m_Camera->SetCoordinate( LerpCoordinate.x, LerpCoordinate.y );
                    m_Camera->UpdateProjectionMatrix( );
                }
            }

            bool DidAdvanced = false;
            if ( m_IsAutoPlay )
            {
                if ( PlayPosition >= NextPlayTilePosition )
                {
                    spdlog::info( "Play position: {}ms, next play tile position: {}ms", PlayPosition, NextPlayTilePosition );

                    TileSpriteSetShared->Advance( );
                    DidAdvanced = true;
                }
            } else
            {
                const Tolerance NoteTolerance = ToTolerance( DeltaTimeToNext );
                if ( NoteTolerance == Tolerance::Miss )
                {
                    spdlog::info( "Node Miss: {} -> {}", PlayPosition, NextPlayTilePosition );

                    TileSpriteSetShared->Advance( );
                    DidAdvanced = true;
                }
                if ( PlayerInteract )
                {
                    DidAdvanced = true;
                    spdlog::info( "Play position: {}ms, next tile position: {}ms, Diff: {}", PlayPosition, NextPlayTilePosition, DeltaTimeToNext );

                    switch ( NoteTolerance )
                    {
                    case Tolerance::Perfect:
                        spdlog::info( "Perfect" );
                        TileSpriteSetShared->Advance( );
                        break;
                    case Tolerance::Good:
                        spdlog::info( "Good" );
                        TileSpriteSetShared->Advance( );
                        break;
                    case Tolerance::Bad:
                        spdlog::info( "Bad" );
                        TileSpriteSetShared->Advance( );
                        break;
                    case Tolerance::EarlyMiss:
                        spdlog::info( "EarlyMiss" );
                        TileSpriteSetShared->Advance( );
                        break;
                    default:
                        DidAdvanced = false;
                        break;
                    }
                }
            }

            if ( DidAdvanced )
            {
                /*
                 *
                 * Add tolerance bar
                 *
                 * */
                auto EarlyMissDegree = (FloatTy) Tolerance::ToleranceBarRange;
                m_ToleranceBar->AddBar( -m_DegreePreMS * DeltaTimeToNext / EarlyMissDegree );

                /*
                 *
                 * Add explosion effect
                 *
                 * */
                const auto TileTransformBefore = TileSpriteSetShared->GetCurrentTileTransform( );
                auto       NewExplosion        = ( AddConcept<SpriteSquareAnimatedTexture>( 512, 512 ) );
                ( *NewExplosion ) << *m_ExplosionSpriteTemplate;
                NewExplosion->SetCoordinate( TileTransformBefore.x, TileTransformBefore.y, TileTransformBefore.z );

                /*
                 *
                 * Switch player sprite
                 *
                 * */
                TryAlterPlayer( );

                const auto& NewTile = TileSpriteSetShared->GetCurrentTileMeta( );

                /*
                 *
                 * Compensating offset for player misalignment generated from off beat hit
                 *
                 * */
                const auto CompensateBeat         = -DeltaTimeToNext / m_MSPB * ( NewTile.ReverseDirection && m_PlayerDirectionClockWise ? -1 : 1 );
                const auto NoteHitCompensateAngle = DirectionVector * CompensateBeat * 3.14159265f;
                const auto RotationDegree         = TileSpriteSetShared->GetCurrentRollRotation( );
                m_InActivePlayerSprite->SetRotation( 0, 0, RotationDegree + 3.14159264f + NoteHitCompensateAngle );

                /*
                 *
                 * Move player for correct location on map
                 *
                 * */
                const auto TileTransform = TileSpriteSetShared->GetCurrentTileTransform( );
                m_InActivePlayerSprite->SetCoordinate( TileTransform.x, TileTransform.y );
                m_ActivePlayerSprite->SetCoordinate( TileTransform.x, TileTransform.y );

                /*
                 *
                 * Move camera to center player
                 *
                 * */
                m_CameraLerp              = 0;
                const auto CameraLocation = m_Camera->GetCoordinate( );
                m_CameraStart             = glm::vec2 { CameraLocation.X, CameraLocation.Y };
                m_CameraEnd               = TileTransform;

                /*
                 *
                 * Tile effect
                 *
                 * */
                if ( NewTile.ReverseDirection )
                {
                    m_PlayerDirectionClockWise = !m_PlayerDirectionClockWise;
                }

                if ( NewTile.BPMChange != 0 )
                {
                    SetBPM( NewTile.BPMChange );
                }
            }
        }
    }
}

void
GameManager::LoadTileSprites( const std::set<uint32_t>& Degrees )
{
    const auto AddDegreeTile = [ & ]( uint32_t Degree ) {
        auto* Sp = m_TileSpriteSetRef.lock( )->RegisterSprite( Degree, std::make_unique<SpriteSquareTexture>( 512, 512 ) );

        Sp->SetShader( m_SpriteShader );
        Sp->SetTexturePath( "Assets/Texture/Tile/" + std::to_string( Degree ) + ".png" );
        Sp->SetupSprite( );
    };

    m_TileSpriteSetRef.lock( )->SetSpritesOrigin( { 512 / 2, 512 / 2 } );

    for ( auto Degree : Degrees )
    {
        AddDegreeTile( Degree );
    }
}

void
GameManager::LoadTileMap( )
{
    auto Map = m_TileSpriteSetRef.lock( );
    for ( int i = 0; i < 40; ++i )
    {
        Map->AddTile( TmpMap[ i ] );
    }
}

void
GameManager::LoadAudio( )
{
    m_PlayingSpeed = 1;

    auto* DDC             = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/Beats.ogg" );
    m_DelayCheckingHandle = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( DDC, true );

    auto* MAC         = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/Papipupipupipa.ogg" );
    m_MainAudioHandle = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( MAC, true, true );
    m_MainAudioHandle.SetSpeed( m_PlayingSpeed );
    SetBPM( 200 * m_PlayingSpeed );
}

void
GameManager::SetupCamera( )
{
    m_Camera = AddConcept<PureConceptCamera>( );

    auto CameraLocked = m_Camera;
    CameraLocked->SetScale( 1 / 2.5F );
    CameraLocked->UpdateProjectionMatrix( );
}

void
GameManager::LoadPlayerSprites( )
{
    m_FBSp = m_InActivePlayerSprite = AddConcept<SpriteSquareTexture>( 512, 512 );

    auto FBSpLocked = m_FBSp.lock( );
    FBSpLocked->SetRotationCenter( 512 / 2 - TileSpriteSet::TileDistance, 512 / 2 );
    FBSpLocked->SetOrigin( 512 / 2 - TileSpriteSet::TileDistance, 512 / 2 );

    FBSpLocked->SetShader( m_SpriteShader );
    FBSpLocked->SetTexturePath( "Assets/Texture/Player/FireBall.png" );
    FBSpLocked->SetActiveCamera( m_Camera.get( ) );
    FBSpLocked->SetupSprite( );

    m_IBSp = m_ActivePlayerSprite = AddConcept<SpriteSquareTexture>( 512, 512 );
    auto IBSpLocked               = m_IBSp.lock( );

    IBSpLocked->SetOrigin( 512 / 2, 512 / 2 );
    IBSpLocked->SetShader( m_SpriteShader );
    IBSpLocked->SetTexturePath( "Assets/Texture/Player/IceBall.png" );
    IBSpLocked->SetActiveCamera( m_Camera.get( ) );
    IBSpLocked->SetupSprite( );
}

bool
GameManager::TryAlterPlayer( )
{

    m_ActivePlayerSprite->SetRotationCenter( 512 / 2 - TileSpriteSet::TileDistance, 512 / 2 );
    m_ActivePlayerSprite->SetOrigin( 512 / 2 - TileSpriteSet::TileDistance, 512 / 2 );

    m_InActivePlayerSprite->SetRotation( );
    m_InActivePlayerSprite->SetRotationCenter( );
    m_InActivePlayerSprite->SetOrigin( 512 / 2, 512 / 2 );

    m_ActivePlayerIsFire = !m_ActivePlayerIsFire;
    std::swap( m_ActivePlayerSprite, m_InActivePlayerSprite );

    return true;
}

bool
GameManager::IsUserPrimaryInteract( )
{
    auto* UserInputHandle = Engine::GetEngine( )->GetUserInputHandle( );
    return UserInputHandle->GetPrimaryKey( ).isPressed || UserInputHandle->GetKeyState( GLFW_KEY_SPACE ).isPressed;
}

void
GameManager::UpdateDeviceOffset( )
{
    constexpr int64_t AudioDefaultOffset = 3000;
    const int64_t     PlayPosition       = m_DelayCheckingHandle.GetCorrectedCurrentAudioOffset( );

    m_UserDeviceOffsetMS = PlayPosition - AudioDefaultOffset;
    spdlog::info( "Offset: {}", m_UserDeviceOffsetMS );
}

void
GameManager::SetBPM( FloatTy BPM )
{
    m_BPM  = BPM;
    m_MSPB = 60 * 1000 / BPM;

    constexpr auto DegreePreBeat = 180;
    m_DegreePreMS                = DegreePreBeat * ( 1 / m_MSPB );
}

GameManager::Tolerance
GameManager::ToTolerance( FloatTy DeltaTime )
{
    const auto AbsDeltaTime  = std::abs( DeltaTime );
    const auto CurrentDegree = m_DegreePreMS * AbsDeltaTime;

    if ( CurrentDegree <= (FloatTy) Tolerance::Perfect )
    {
        return Tolerance::Perfect;
    } else if ( CurrentDegree <= (FloatTy) Tolerance::Good )
    {
        return Tolerance::Good;
    } else if ( CurrentDegree <= (FloatTy) Tolerance::Bad )
    {
        return Tolerance::Bad;
    } else if ( DeltaTime > 0 )
    {
        if ( CurrentDegree <= (FloatTy) Tolerance::EarlyMiss )
        {
            return Tolerance::EarlyMiss;
        } else
        {
            return Tolerance::None;
        }
    } else
    {
        return Tolerance::Miss;
    }
}

void
GameManager::SetupExplosionSpriteTemplate( )
{
    REQUIRED_IF( m_SpriteShader, assert( false && "Explosion shader not loaded" ) )
    {
        m_ExplosionSpriteTemplate = std::make_unique<SpriteSquareAnimatedTexture>( 512, 512 );

        m_ExplosionSpriteTemplate->SetOrigin( 512 / 2, 512 / 2 );
        m_ExplosionSpriteTemplate->SetShader( m_SpriteShader );
        m_ExplosionSpriteTemplate->SetTexturePath( "Assets/Texture/explosion.png" );
        m_ExplosionSpriteTemplate->SetActiveCamera( m_Camera.get( ) );

        // Animation setting
        m_ExplosionSpriteTemplate->SetTextureGrid( 8, 8 );
        m_ExplosionSpriteTemplate->SetFrameTime( 0.005F );
        m_ExplosionSpriteTemplate->SetRepeat( false );

        m_ExplosionSpriteTemplate->SetupSprite( );
    }
}

void
GameManager::SetupShader( )
{
    m_SpriteShader = Engine::GetEngine( )->GetGlobalResourcePool( )->GetShared<Shader>( "DefaultTextureShader" );
}

void
GameManager::LoadToleranceSprite( )
{
    m_ToleranceBar = AddConcept<ToleranceBar>( 400, 30 );

    m_ToleranceBar->SetRotationCenter( 400 / 2, 30 / 2 );
    m_ToleranceBar->SetOrigin( 400 / 2, 30 / 2 );

    m_ToleranceBar->SetShader( m_SpriteShader );
    m_ToleranceBar->SetTexturePath( "Assets/Texture/tolerance.png" );
    m_ToleranceBar->SetActiveCamera( m_Camera.get( ) );
    m_ToleranceBar->SetupSprite( );
}
