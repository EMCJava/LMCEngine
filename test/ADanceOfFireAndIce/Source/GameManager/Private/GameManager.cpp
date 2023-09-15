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
    { 270, 3293, true },
    { 90, 3443, true },
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
    { 315, 7868, true },
    { 45, 7943 },
    { 315, 8018, true },
    { 90, 8093 },
    { 270, 8243, true },
    { 90, 8393 },
    { 270, 8543, true },
    { 90, 8693 },
    { 315, 8843, true },
    { 45, 8918 },
    { 270, 8993, true },
    { 90, 9143 },
    { 315, 9293, true },
    { 45, 9368 },
    { 270, 9443, true },
    { 180, 9593 },
    { 180, 9893 },
    { 180, 10193 },
    { 45, 10493 },
    { 135, 10568 },
    { 180, 10793 },
    { 270, 11093, true },
    { 90, 11243 },
    { 270, 11393, true },
    { 90, 11543 },
    { 270, 11693, true },
    { 180, 11843 },
    { 90, 12143 },
    { 180, 12293 },
    { 90, 12593 },
    { 90, 12743 },
    { 270, 12893, true },
    { 270, 13043, true },
    { 180, 13193 },
    { 90, 13493 },
    { 90, 13643 },
    { 270, 13793, true },
    { 270, 13943, true },
    { 90, 14093 },
    { 180, 14243 },
    { 270, 14543, true },
    { 180, 14693 },
    { 90, 14993 },
    { 270, 15143, true },
    { 90, 15293 },
    { 270, 15443, true },
    { 180, 15593 },
    { 90, 15893 },
    { 270, 16043, true },
    { 90, 16193 },
    { 90, 16343 },
    { 270, 16493, true },
    { 180, 16643 },
    { 270, 16943, true },
    { 90, 17093 },
    { 270, 17243, true },
    { 90, 17393 },
    { 270, 17543, true },
    { 270, 17693, true },
    { 180, 17843 },
    { 270, 18143, true },
    { 90, 18293 },
    { 90, 18443 },
    { 270, 18593, true },
    { 270, 18743, true },
    { 90, 18893 },
    { 180, 19043, true },
    { 90, 19343 },
    { 270, 19493, true },
    { 90, 19643 },
    { 90, 19793 },
    { 270, 19943, true },
    { 270, 20093, true },
    { 90, 20243, true },
    { 270, 20393, true },
    { 45, 20543, true },
    { 315, 20618, true },
    { 90, 20693 },
    { 90, 20843 },
    { 315, 20993, true },
    { 45, 21068 },
    { 270, 21143, true },
    { 90, 21293 },
    { 270, 21443, true },
    { 270, 21593, true },
    { 45, 21743, true },
    { 315, 21818, true },
    { 90, 21893 },
    { 90, 22043 },
    { 315, 22193, true },
    { 45, 22268 },
    { 315, 22343, true },
    { 45, 22418, true },
    { 270, 22493, true },
    { 90, 22643 },
    { 270, 22793, true },
    { 45, 22943 },
    { 315, 23018, true },
    { 90, 23093 },
    { 270, 23243, true },
    { 45, 23393, true },
    { 315, 23468, true },
    { 45, 23543, true },
    { 315, 23618, true },
    { 90, 23693 },
    { 90, 23843 },
    { 270, 23993, true },
    { 45, 24143 },
    { 315, 24218, true },
    { 90, 24293 },
    { 270, 24443, true },
    { 45, 24593 },
    { 315, 24668, true },
    { 90, 24743 },
    { 315, 24893, true },
    { 45, 24968 },
    { 270, 25043, true },
    { 90, 25193 },
    { 315, 25343, true },
    { 45, 25418 },
    { 270, 25493, true },
    { 270, 25643, true },
    { 45, 25793 },
    { 315, 25868, true },
    { 45, 25943 },
    { 315, 26018, true },
    { 90, 26093 },
    { 270, 26243, true },
    { 45, 26393 },
    { 315, 26468, true },
    { 90, 26543 },
    { 90, 26693, true },
    { 315, 26843, true },
    { 45, 26918, true },
    { 315, 26993, true },
    { 45, 27068, true },
    { 270, 27143, true },
    { 270, 27293, true },
    { 90, 27443 },
    { 270, 27593, true },
    { 45, 27743, true },
    { 315, 27818, true },
    { 90, 27893 },
    { 270, 28043, true },
    { 45, 28193, true },
    { 315, 28268, true },
    { 90, 28343 },
    { 270, 28493, true },
    { 270, 28643, true },
    { 45, 28793, true },
    { 315, 28868, true },
    { 45, 28943 },
    { 315, 29018, true },
    { 90, 29093, true },
    { 90, 29243 },
    { 180, 29393 },
    { 90, 29693 },
    { 270, 29843, true },
    { 90, 29993 },
    { 315, 30143, true },
    { 45, 30218 },
    { 270, 30293, true },
    { 90, 30443 },
    { 225, 30593, true },
    { 45, 30818 },
    { 270, 30893, true },
    { 90, 31043 },
    { 270, 31193, true },
    { 45, 31343, true },
    { 315, 31418, true },
    { 90, 31493 },
    { 270, 31643, true },
    { 90, 31793 },
    { 315, 31943, true },
    { 45, 32018 },
    { 270, 32093, true },
    { 270, 32243, true },
    { 90, 32393 },
    { 315, 32543, true },
    { 45, 32618 },
    { 270, 32693, true },
    { 270, 32843, true },
    { 135, 32993, true },
    { 45, 33218, true },
    { 270, 33293, true },
    { 270, 33443, true },
    { 90, 33593, true },
    { 315, 33743, true },
    { 45, 33818 },
    { 270, 33893, true },
    { 90, 34043 },
    { 315, 34193, true },
    { 45, 34268, true },
    { 315, 34343, true },
    { 45, 34418, true },
    { 270, 34493, true },
    { 90, 34643 },
    { 90, 34793 },
    { 315, 34943, true },
    { 45, 35018 },
    { 270, 35093, true },
    { 90, 35243 },
    { 315, 35393, true },
    { 45, 35468, true },
    { 315, 35543, true },
    { 45, 35618 },
    { 270, 35693, true },
    { 90, 35843 },
    { 315, 35993, true },
    { 45, 36068 },
    { 315, 36143, true },
    { 45, 36218 },
    { 315, 36293, true },
    { 45, 36368 },
    { 270, 36443, true },
    { 45, 36593, true },
    { 315, 36668, true },
    { 45, 36743 },
    { 315, 36818, true },
    { 45, 36893 },
    { 315, 36968, true },
    { 90, 37043 },
    { 90, 37193 },
    { 315, 37343, true },
    { 45, 37418 },
    { 315, 37493, true },
    { 45, 37568 },
    { 315, 37643, true },
    { 45, 37718 },
    { 315, 37793, true },
    { 45, 37868 },
    { 315, 37943, true },
    { 45, 38018 },
    { 315, 38093, true },
    { 45, 38168 },
    { 315, 38243, true },
    { 45, 38318 },
    { 270, 38393, true },
    { 90, 38543 },
    { 90, 38693 },
    { 270, 38843, true },
    { 180, 38993 },
    { 90, 39293 },
    { 270, 39443, true },
    { 180, 39593 },
    { 180, 39893 },
    { 180, 40193 },
    { 180, 40493 },
    { 180, 40793 },
    { 180, 41093 },
    { 180, 41393 },
    { 270, 41693, true },
    { 90, 41843 },
    { 180, 41993 },
    { 180, 42293 },
    { 180, 42593 },
    { 180, 42893 },
    { 270, 43193, true },
    { 45, 43343 },
    { 315, 43418, true },
    { 90, 43493 },
    { 270, 43643, true },
    { 45, 43793, true },
    { 315, 43868, true },
    { 45, 43943 },
    { 315, 44018, true },
    { 90, 44093 },
    { 270, 44243, true },
    { 180, 44393 },
    { 180, 44693 },
    { 180, 44993 },
    { 180, 45293 },
    { 180, 45593 },
    { 180, 45893 },
    { 180, 46193 },
    { 90, 46493 },
    { 90, 46643, true },
    { 180, 46793 },
    { 180, 47093 },
    { 180, 47393 },
    { 180, 47693 },
    { 270, 47993, true },
    { 45, 48143, true },
    { 315, 48218, true },
    { 90, 48293 },
    { 90, 48443 },
    { 315, 48593, true },
    { 45, 48668 },
    { 315, 48743, true },
    { 45, 48818 },
    { 315, 48893, true },
    { 45, 48968 },
    { 270, 49043, true },
    { 225, 49193, true },
    { 315, 49418, true },
    { 90, 49493 },
    { 90, 49643 },
    { 315, 49793, true },
    { 90, 49868 },
    { 315, 50018, true },
    { 90, 50093, true },
    { 270, 50243, true },
    { 45, 50393, true },
    { 315, 50468, true },
    { 90, 50543, true },
    { 315, 50693, true },
    { 45, 50768, true },
    { 270, 50843, true },
    { 45, 50993 },
    { 315, 51068, true },
    { 45, 51143 },
    { 315, 51218, true },
    { 90, 51293 },
    { 270, 51443, true },
    { 90, 51593 },
    { 315, 51743, true },
    { 45, 51818 },
    { 270, 51893, true },
    { 90, 52043 },
    { 315, 52193, true },
    { 45, 52268 },
    { 315, 52343, true },
    { 45, 52418 },
    { 270, 52493, true },
    { 270, 52643, true },
    { 90, 52793, true },
    { 315, 52943, true },
    { 45, 53018, true },
    { 270, 53093, true },
    { 270, 53243, true },
    { 45, 53393 },
    { 315, 53468, true },
    { 45, 53543 },
    { 315, 53618, true },
    { 45, 53693 },
    { 315, 53768, true },
    { 90, 53843, true },
    { 135, 53993, true },
    { 45, 54218 },
    { 270, 54293, true },
    { 270, 54443, true },
    { 45, 54593, true },
    { 270, 54668, true },
    { 45, 54818 },
    { 270, 54893, true },
    { 90, 55043 },
    { 315, 55193, true },
    { 45, 55268 },
    { 270, 55343, true },
    { 45, 55493 },
    { 315, 55568, true },
    { 90, 55643, true },
    { 300, 55793, true },
    { 60, 55893 },
    { 300, 55993, true },
    { 60, 56093 },
    { 300, 56193, true },
    { 60, 56293 },
    { 270, 56393, true },
    { 45, 56543 },
    { 315, 56618, true },
    { 90, 56693 },
    { 270, 56843, true },
    { 45, 56993 },
    { 315, 57068, true },
    { 45, 57143 },
    { 315, 57218, true },
    { 90, 57293 },
    { 270, 57443, true },
    { 90, 57593 },
    { 315, 57743, true },
    { 45, 57818 },
    { 270, 57893, true },
    { 270, 58043, true },
    { 45, 58193, true },
    { 315, 58268, true },
    { 90, 58343, true },
    { 330, 58493, true },
    { 30, 58543, true },
    { 330, 58593, true },
    { 90, 58643 },
    { 180, 58793 },
    { 270, 59093, true },
    { 90, 59243 },
    { 270, 59393, true },
    { 45, 59543, true },
    { 315, 59618, true },
    { 90, 59693 },
    { 90, 59843 },
    { 270, 59993, true },
    { 45, 60143 },
    { 315, 60218, true },
    { 90, 60293 },
    { 90, 60443 },
    { 315, 60593, true },
    { 45, 60668 },
    { 315, 60743, true },
    { 45, 60818, true },
    { 270, 60893, true },
    { 90, 61043 },
    { 90, 61193 },
    { 270, 61343, true },
    { 270, 61493, true },
    { 45, 61643, true },
    { 315, 61718, true },
    { 90, 61793, true },
    { 315, 61943, true },
    { 45, 62018 },
    { 315, 62093, true },
    { 45, 62168 },
    { 270, 62243, true },
    { 180, 62393 },
    { 60, 62693 },
    { 300, 62793, true },
    { 60, 62893, true },
    { 300, 62993, true },
    { 60, 63093 },
    { 300, 63193, true },
    { 60, 63293 },
    { 300, 63393, true },
    { 60, 63493, true },
    { 240, 63593, true },
    { 60, 63793 },
    { 120, 63893 },
    { 300, 64093, true },
    { 120, 64193 },
    { 300, 64393, true },
    { 120, 64493, true },
    { 300, 64693, true },
    { 120, 64793 },
    { 60, 64993 },
    { 300, 65093, true },
    { 60, 65193, true },
    { 300, 65293, true },
    { 120, 65393 },
    { 60, 65593 },
    { 300, 65693, true },
    { 60, 65793, true },
    { 300, 65893, true },
    { 120, 65993 },
    { 60, 66193 },
    { 240, 66293, true },
    { 300, 66493, true },
    { 120, 66593 },
    { 60, 66793, true },
    { 240, 66893, true },
    { 300, 67093, true },
    { 120, 67193 },
    { 60, 67393, true },
    { 300, 67493, true },
    { 60, 67593, true },
    { 300, 67693, true },
    { 120, 67793 },
    { 300, 67993, true },
    { 180, 68093 },
    { 120, 68393 },
    { 300, 68593, true },
    { 240, 68693, true },
    { 60, 68893, true },
    { 120, 68993 },
    { 300, 69193, true },
    { 240, 69293, true },
    { 60, 69493, true },
    { 120, 69593 },
    { 300, 69793, true },
    { 60, 69893 },
    { 300, 69993, true },
    { 60, 70093 },
    { 240, 70193, true },
    { 300, 70393, true },
    { 60, 70493, true },
    { 300, 70593, true },
    { 60, 70693, true },
    { 120, 70793, true },
    { 300, 70993, true },
    { 120, 71093 },
    { 300, 71293, true },
    { 120, 71393 },
    { 60, 71593 },
    { 240, 71693, true },
    { 60, 71893 },
    { 240, 71993, true },
    { 300, 72193, true },
    { 60, 72293 },
    { 300, 72393, true },
    { 60, 72493 },
    { 270, 72593, true },
    { 180, 72743 },
    { 180, 73043 },
    { 45, 73343 },
    { 315, 73418, true },
    { 90, 73493 },
    { 90, 73643 },
    { 315, 73793, true },
    { 45, 73868 },
    { 270, 73943, true },
    { 90, 74093 },
    { 270, 74243, true },
    { 90, 74393 },
    { 315, 74543, true },
    { 45, 74618 },
    { 270, 74693, true },
    { 270, 74843, true },
    { 45, 74993 },
    { 315, 75068, true },
    { 45, 75143 },
    { 315, 75218, true },
    { 90, 75293, true },
    { 180, 75443 },
    { 315, 75743, true },
    { 45, 75818 },
    { 270, 75893, true },
    { 90, 76043 },
    { 315, 76193, true },
    { 45, 76268 },
    { 315, 76343, true },
    { 45, 76418 },
    { 270, 76493, true },
    { 90, 76643 },
    { 270, 76793, true },
    { 45, 76943 },
    { 315, 77018, true },
    { 90, 77093 },
    { 90, 77243 },
    { 315, 77393, true },
    { 45, 77468 },
    { 315, 77543, true },
    { 45, 77618 },
    { 270, 77693, true },
    { 180, 77843 },
    { 45, 78143 },
    { 315, 78218, true },
    { 90, 78293, true },
    { 270, 78443, true },
    { 45, 78593, true },
    { 315, 78668, true },
    { 45, 78743, true },
    { 315, 78818, true },
    { 90, 78893, true },
    { 270, 79043, true },
    { 315, 79193, true },
    { 45, 79268, true },
    { 270, 79343, true },
    { 90, 79493, true },
    { 315, 79643, true },
    { 45, 79718, true },
    { 315, 79793, true },
    { 45, 79868 },
    { 270, 79943, true },
    { 90, 80093 },
    { 270, 80243, true },
    { 270, 80393, true },
    { 45, 80543, true },
    { 315, 80618, true },
    { 90, 80693 },
    { 270, 80843, true },
    { 45, 80993 },
    { 315, 81068, true },
    { 90, 81143, true },
    { 270, 81293, true },
    { 45, 81443, true },
    { 315, 81518, true },
    { 45, 81593 },
    { 315, 81668, true },
    { 45, 81743, true },
    { 315, 81818, true },
    { 90, 81893, true },
    { 90, 82043 },
    { 180, 82193 },
    { 90, 82493 },
    { 270, 82643, true },
    { 135, 82793 },
    { 315, 83018, true },
    { 90, 83093 },
    { 315, 83243, true },
    { 45, 83318 },
    { 225, 83393, true },
    { 45, 83618 },
    { 270, 83693, true },
    { 90, 83843, true },
    { 270, 83993, true },
    { 45, 84143 },
    { 315, 84218, true },
    { 90, 84293 },
    { 90, 84443, true },
    { 270, 84593, true },
    { 45, 84743, true },
    { 270, 84818, true },
    { 45, 84968, true },
    { 270, 85043, true },
    { 225, 85193, true },
    { 315, 85418, true },
    { 90, 85493, true },
    { 315, 85643, true },
    { 45, 85718, true },
    { 225, 85793, true },
    { 315, 86018, true },
    { 90, 86093, true },
    { 90, 86243 },
    { 270, 86393, true },
    { 45, 86543 },
    { 315, 86618, true },
    { 90, 86693 },
    { 270, 86843, true },
    { 45, 86993 },
    { 315, 87068, true },
    { 45, 87143 },
    { 315, 87218, true },
    { 90, 87293 },
    { 270, 87443, true },
    { 45, 87593, true },
    { 315, 87668, true },
    { 45, 87743, true },
    { 315, 87818, true },
    { 90, 87893 },
    { 90, 88043 },
    { 315, 88193, true },
    { 45, 88268 },
    { 315, 88343, true },
    { 45, 88418 },
    { 270, 88493, true },
    { 270, 88643, true },
    { 45, 88793 },
    { 315, 88868, true },
    { 45, 88943, true },
    { 315, 89018, true },
    { 90, 89093, true },
    { 315, 89243, true },
    { 45, 89318, true },
    { 315, 89393, true },
    { 45, 89468, true },
    { 315, 89543, true },
    { 45, 89618 },
    { 315, 89693, true },
    { 45, 89768 },
    { 270, 89843, true },
    { 45, 89993, true },
    { 315, 90068, true },
    { 45, 90143, true },
    { 315, 90218, true },
    { 45, 90293 },
    { 315, 90368, true },
    { 45, 90443, true },
    { 315, 90518, true },
    { 45, 90593, true },
    { 315, 90668, true },
    { 45, 90743, true },
    { 315, 90818, true },
    { 45, 90893 },
    { 315, 90968, true },
    { 45, 91043 },
    { 315, 91118, true },
    { 120, 91193, true },
    { 300, 91393, true },
    { 120, 91493 },
    { 60, 91693 },
    { 240, 91793, true },
    { 300, 91993, true },
    { 120, 92093 },
    { 300, 92293, true },
    { 180, 92393 },
    { 180, 92693 },
    { 60, 92993 },
    { 300, 93093, true },
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
    { 270, 95843, true },
    { 180, 95993 },
    { 180, 96293 },
    { 180, 96593 },
    { 180, 96893 },
    { 90, 97193 },
    { 315, 97343, true },
    { 45, 97418 },
    { 270, 97493, true },
    { 90, 97643 },
    { 315, 97793, true },
    { 45, 97868 },
    { 315, 97943, true },
    { 45, 98018 },
    { 270, 98093, true },
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
    { 315, 102668, true },
    { 45, 102743 },
    { 315, 102818, true },
    { 45, 102893 },
    { 315, 102968, true },
    { 90, 103043 },
    { 225, 103193, true },
    { 315, 103418, true },
    { 90, 103493 },
    { 270, 103643, true },
    { 45, 103793 },
    { 270, 103868, true },
    { 45, 104018, true },
    { 270, 104093, true },
    { 270, 104243, true },
    { 45, 104393 },
    { 315, 104468, true },
    { 90, 104543 },
    { 315, 104693, true },
    { 45, 104768 },
    { 270, 104843, true },
    { 45, 104993, true },
    { 315, 105068, true },
    { 45, 105143 },
    { 315, 105218, true },
    { 90, 105293 },
    { 270, 105443, true },
    { 90, 105593 },
    { 315, 105743, true },
    { 45, 105818 },
    { 270, 105893, true },
    { 270, 106043, true },
    { 45, 106193 },
    { 315, 106268, true },
    { 45, 106343 },
    { 315, 106418, true },
    { 90, 106493 },
    { 270, 106643, true },
    { 270, 106793, true },
    { 45, 106943 },
    { 315, 107018, true },
    { 90, 107093 },
    { 270, 107243, true },
    { 45, 107393, true },
    { 315, 107468, true },
    { 45, 107543 },
    { 315, 107618, true },
    { 45, 107693 },
    { 315, 107768, true },
    { 90, 107843 },
    { 135, 107993 },
    { 45, 108218 },
    { 270, 108293, true },
    { 270, 108443, true },
    { 45, 108593, true },
    { 270, 108668, true },
    { 45, 108818 },
    { 270, 108893, true },
    { 270, 109043, true },
    { 45, 109193 },
    { 315, 109268, true },
    { 90, 109343, true },
    { 315, 109493, true },
    { 45, 109568 },
    { 270, 109643, true },
    { 60, 109793, true },
    { 300, 109893, true },
    { 60, 109993 },
    { 300, 110093, true },
    { 60, 110193 },
    { 300, 110293, true },
    { 90, 110393 },
    { 315, 110543, true },
    { 45, 110618 },
    { 270, 110693, true },
    { 270, 110843, true },
    { 45, 110993 },
    { 315, 111068, true },
    { 45, 111143 },
    { 315, 111218, true },
    { 90, 111293 },
    { 90, 111443 },
    { 270, 111593, true },
    { 45, 111743 },
    { 315, 111818, true },
    { 90, 111893 },
    { 90, 112043 },
    { 315, 112193, true },
    { 45, 112268 },
    { 270, 112343, true },
    { 30, 112493 },
    { 330, 112543, true },
    { 30, 112593 },
    { 270, 112643, true },
    { 180, 112793 },
    { 90, 113093 },
    { 270, 113243, true },
    { 270, 113393, true },
    { 45, 113543, true },
    { 315, 113618, true },
    { 90, 113693 },
    { 90, 113843 },
    { 270, 113993, true },
    { 45, 114143 },
    { 315, 114218, true },
    { 90, 114293 },
    { 90, 114443 },
    { 315, 114593, true },
    { 45, 114668 },
    { 315, 114743, true },
    { 45, 114818, true },
    { 270, 114893, true },
    { 270, 115043, true },
    { 180, 115193 },
    { 270, 115493, true },
    { 45, 115643 },
    { 315, 115718, true },
    { 90, 115793 },
    { 315, 115943, true },
    { 45, 116018 },
    { 315, 116093, true },
    { 45, 116168 },
    { 270, 116243, true },
    { 270, 116393, true },
    { 90, 116543 },
    { 315, 116693, true },
    { 45, 116768 },
    { 270, 116843, true },
    { 60, 116993 },
    { 300, 117093, true },
    { 60, 117193 },
    { 300, 117293, true },
    { 60, 117393 },
    { 300, 117493, true },
    { 90, 117593 },
    { 315, 117743, true },
    { 45, 117818 },
    { 270, 117893, true },
    { 45, 118043 },
    { 315, 118118, true },
    { 45, 118193 },
    { 315, 118268, true },
    { 45, 118343 },
    { 315, 118418, true },
    { 90, 118493 },
    { 90, 118643 },
    { 315, 118793, true },
    { 45, 118868 },
    { 315, 118943, true },
    { 45, 119018 },
    { 270, 119093, true },
    { 45, 119243 },
    { 315, 119318, true },
    { 45, 119393 },
    { 315, 119468, true },
    { 45, 119543 },
    { 315, 119618, true },
    { 45, 119693 },
    { 315, 119768, true },
    { 90, 119843 },
    { 315, 119993, true },
    { 45, 120068 },
    { 315, 120143, true },
    { 45, 120218 },
    { 315, 120293, true },
    { 45, 120368 },
    { 315, 120443, true },
    { 45, 120518 },
    { 315, 120593, true },
    { 45, 120668 },
    { 315, 120743, true },
    { 45, 120818 },
    { 315, 120893, true },
    { 45, 120968 },
    { 315, 121043, true },
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
    { 270, 133343, true },
    { 90, 133493 },
    { 270, 133643, true },
    { 270, 133793, true },
    { 90, 133943 },
    { 270, 134093, true },
    { 270, 134243, true },
    { 90, 134393, true },
    { 270, 134543, true },
    { 90, 134693 },
    { 270, 134843, true },
    { 45, 134993 },
    { 315, 135068, true },
    { 45, 135143, true },
    { 315, 135218, true },
    { 180, 135293 },
    { 135, 135593 },
    { 45, 135818 },
    { 270, 135893, true },
    { 90, 136043 },
    { 315, 136193, true },
    { 90, 136268, true },
    { 315, 136418, true },
    { 90, 136493, true },
    { 90, 136643, true },
    { 315, 136793, true },
    { 45, 136868 },
    { 270, 136943, true },
    { 45, 137093, true },
    { 315, 137168, true },
    { 90, 137243 },
    { 315, 137393, true },
    { 45, 137468 },
    { 315, 137543, true },
    { 45, 137618 },
    { 270, 137693, true },
    { 90, 137843 },
    { 90, 137993 },
    { 315, 138143, true },
    { 45, 138218 },
    { 270, 138293, true },
    { 90, 138443 },
    { 315, 138593, true },
    { 45, 138668 },
    { 315, 138743, true },
    { 45, 138818 },
    { 270, 138893, true },
    { 90, 139043 },
    { 90, 139193 },
    { 315, 139343, true },
    { 45, 139418 },
    { 270, 139493, true },
    { 90, 139643 },
    { 315, 139793, true },
    { 45, 139868 },
    { 315, 139943, true },
    { 45, 140018 },
    { 315, 140093, true },
    { 45, 140168 },
    { 270, 140243, true },
    { 225, 140393, true },
    { 45, 140618 },
    { 270, 140693, true },
    { 270, 140843, true },
    { 45, 140993 },
    { 270, 141068, true },
    { 45, 141218, true },
    { 270, 141293, true },
    { 270, 141443, true },
    { 45, 141593 },
    { 315, 141668, true },
    { 90, 141743 },
    { 315, 141893, true },
    { 45, 141968, true },
    { 270, 142043, true },
    { 45, 142193, true },
    { 315, 142268, true },
    { 45, 142343 },
    { 315, 142418, true },
    { 45, 142493 },
    { 315, 142568, true },
    { 45, 142643 },
    { 315, 142718, true },
    { 90, 142793 },
    { 315, 142943, true },
    { 45, 143018 },
    { 270, 143093, true },
    { 90, 143243 },
    { 315, 143393, true },
    { 45, 143468 },
    { 315, 143543, true },
    { 45, 143618 },
    { 270, 143693, true },
    { 270, 143843, true },
    { 90, 143993 },
    { 315, 144143, true },
    { 45, 144218 },
    { 270, 144293, true },
    { 90, 144443 },
    { 315, 144593, true },
    { 45, 144668 },
    { 315, 144743, true },
    { 45, 144818 },
    { 330, 144893, true },
    { 30, 144943 },
    { 330, 144993, true },
    { 90, 145043 },
    { 180, 145193, true },
    { 180, 145493 },
    { 180, 145793 },
    { 180, 146093 },
    { 180, 146393 },
    { 180, 146693 },
    { 180, 146993 },
    { 90, 147293 },
    { 270, 147443, true },
    { 180, 147593 },
    { 180, 147893 },
    { 180, 148193 },
    { 180, 148493 },
    { 90, 148793 },
    { 90, 148943 },
    { 270, 149093, true },
    { 90, 149243 },
    { 90, 149393 },
    { 270, 149543, true },
    { 270, 149693, true },
    { 90, 149843, true },
    { 135, 149993 },
    { 315, 150218, true },
    { 90, 150293 },
    { 90, 150443, true },
    { 225, 150593, true },
    { 315, 150818, true },
    { 90, 150893 },
    { 90, 151043 },
    { 225, 151193, true },
    { 315, 151418, true },
    { 90, 151493 },
    { 270, 151643, true },
    { 135, 151793, true },
    { 45, 152018 },
    { 270, 152093, true },
    { 90, 152243 },
    { 225, 152393, true },
    { 315, 152618, true },
    { 90, 152693 },
    { 270, 152843, true },
    { 90, 152993, true },
    { 270, 153143, true },
    { 90, 153293 },
    { 270, 153443, true },
    { 180, 153593 },
    { 315, 153893, true },
    { 45, 153968 },
    { 270, 154043, true },
    { 45, 154193 },
    { 315, 154268, true },
    { 45, 154343 },
    { 315, 154418, true },
    { 45, 154493 },
    { 315, 154568, true },
    { 90, 154643 },
    { 225, 154793, true },
    { 45, 155018 },
    { 270, 155093, true },
    { 270, 155243, true },
    { 45, 155393, true },
    { 315, 155468, true },
    { 45, 155543, true },
    { 315, 155618, true },
    { 90, 155693 },
    { 270, 155843, true },
    { 45, 155993, true },
    { 315, 156068, true },
    { 45, 156143 },
    { 315, 156218, true },
    { 45, 156293 },
    { 315, 156368, true },
    { 90, 156443 },
    { 315, 156593, true },
    { 45, 156668 },
    { 315, 156743, true },
    { 45, 156818 },
    { 315, 156893, true },
    { 45, 156968 },
    { 315, 157043, true },
    { 45, 157118 },
    { 270, 157193, true },
    { 45, 157343 },
    { 315, 157418, true },
    { 90, 157493 },
    { 270, 157643, true },
    { 45, 157793 },
    { 315, 157868, true },
    { 45, 157943 },
    { 315, 158018, true },
    { 90, 158093 },
    { 270, 158243, true },
    { 45, 158393 },
    { 315, 158468, true },
    { 90, 158543 },
    { 315, 158693, true },
    { 45, 158768 },
    { 270, 158843, true },
    { 45, 158993 },
    { 315, 159068, true },
    { 45, 159143 },
    { 315, 159218, true },
    { 45, 159293 },
    { 315, 159368, true },
    { 90, 159443 },
    { 270, 159593, true },
    { 45, 159743 },
    { 315, 159818, true },
    { 90, 159893 },
    { 315, 160043, true },
    { 45, 160118 },
    { 315, 160193, true },
    { 45, 160268, true },
    { 315, 160343, true },
    { 45, 160418, true },
    { 270, 160493, true },
    { 270, 160643, true },
    { 45, 160793 },
    { 315, 160868, true },
    { 45, 160943 },
    { 315, 161018, true },
    { 30, 161093 },
    { 330, 161143, true },
    { 30, 161193 },
    { 270, 161243, true },
    { 60, 161393 },
    { 300, 161493, true },
    { 60, 161593 },
    { 300, 161693, true },
    { 60, 161793 },
    { 300, 161893, true },
    { 90, 161993 },
    { 315, 162143, true },
    { 45, 162218 },
    { 315, 162293, true },
    { 45, 162368 },
    { 270, 162443, true },
    { 45, 162593 },
    { 315, 162668, true },
    { 45, 162743 },
    { 315, 162818, true },
    { 45, 162893 },
    { 315, 162968, true },
    { 90, 163043 },
    { 90, 163193, true },
    { 315, 163343, true },
    { 45, 163418 },
    { 270, 163493, true },
    { 45, 163643 },
    { 315, 163718, true },
    { 45, 163793 },
    { 315, 163868, true },
    { 45, 163943 },
    { 315, 164018, true },
    { 30, 164093 },
    { 330, 164143, true },
    { 30, 164193 },
    { 270, 164243, true },
    { 180, 164393 },
    { 90, 164693 },
    { 90, 164843 },
    { 270, 164993, true },
    { 45, 165143, true },
    { 315, 165218, true },
    { 90, 165293 },
    { 270, 165443, true },
    { 270, 165593, true },
    { 90, 165743, true },
    { 270, 165893, true },
    { 270, 166043, true },
    { 45, 166193, true },
    { 315, 166268, true },
    { 45, 166343, true },
    { 315, 166418, true },
    { 90, 166493 },
    { 90, 166643 },
    { 270, 166793, true },
    { 270, 166943, true },
    { 90, 167093 },
    { 315, 167243, true },
    { 45, 167318 },
    { 270, 167393, true },
    { 270, 167543, true },
    { 45, 167693 },
    { 315, 167768, true },
    { 90, 167843 },
    { 270, 167993, true },
    { 90, 168143 },
    { 270, 168293, true },
    { 90, 168443 },
    { 315, 168593, true },
    { 45, 168668 },
    { 270, 168743, true },
    { 90, 168893 },
    { 270, 169043, true },
    { 180, 169193 },
    { 90, 169493 },
    { 270, 169643, true },
    { 90, 169793 },
    { 270, 169943, true },
    { 180, 170093 },
    { 270, 170393, true },
    { 90, 170543 },
    { 90, 170693 },
    { 270, 170843, true },
    { 270, 170993, true },
    { 90, 171143 },
    { 180, 171293, true },
    { 90, 171593 },
    { 270, 171743, true },
    { 270, 171893, true },
    { 45, 172043, true },
    { 315, 172118, true },
    { 90, 172193 },
    { 90, 172343 },
    { 315, 172493, true },
    { 45, 172568 },
    { 270, 172643, true },
    { 180, 172793 },
    { 180, 173093 },
    { 180, 173393 },
    { 45, 173693 },
    { 360, 173768, true }
};

}

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    SetupCamera( );
    LoadAudio( );

    m_TileSpriteSetRef = AddConcept<TileSpriteSet>( );

    SetupShader( );

    LoadPlayerSprites( );

    LoadTileSprites( { 360, 330, 315, 300, 270, 240, 225, 180, 135, 120, 90, 60, 45, 30, 0 } );
    LoadTileMap( );

    SetupExplosionSpriteTemplate( );
    LoadToleranceSprite( );

    const auto& Button = AddConcept<RectButton>( 0, 100 );
    Button->SetPressReactColor( glm::vec4 { 1, 1, 0.5, 1 } );
    Button->SetCallback( []( ) { spdlog::info( "Button pressed." ); } );

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
                m_CameraLerp += DeltaSecond * 8;
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

            constexpr auto Mapping = false;
            if ( Mapping )
            {
                bool        DidMove    = false;
                const auto& RightStage = Engine::GetEngine( )->GetUserInputHandle( )->GetKeyState( GLFW_KEY_RIGHT );
                if ( RightStage.isPressed || RightStage.isRepeat )
                {
                    TileSpriteSetShared->AddTile( TmpMap[ TileSpriteSetShared->GetTileCount( ) ] );
                    TileSpriteSetShared->Advance( );

                    DidMove = true;
                }

                const auto& LeftStage = Engine::GetEngine( )->GetUserInputHandle( )->GetKeyState( GLFW_KEY_LEFT );
                if ( LeftStage.isPressed || LeftStage.isRepeat )
                {
                    TileSpriteSetShared->Retreat( );
                    TileSpriteSetShared->RemoveLastTile( );

                    DidMove = true;
                }

                if ( Engine::GetEngine( )->GetUserInputHandle( )->GetKeyState( GLFW_KEY_R ).isPressed )
                {
                    TileSpriteSetShared->Retreat( );
                    TileSpriteSetShared->RemoveLastTile( );

                    auto& Tail            = TmpMap[ TileSpriteSetShared->GetTileCount( ) ];
                    Tail.ReverseDirection = !Tail.ReverseDirection;
                    Tail.Degree           = 360 - Tail.Degree;

                    if ( TileSpriteSetShared->GetTileCount( ) < TmpMap.size( ) )
                    {
                        TmpMap[ TileSpriteSetShared->GetTileCount( ) ].ReverseDirection = true;
                    }

                    TileSpriteSetShared->AddTile( Tail );
                    TileSpriteSetShared->Advance( );
                }

                if ( Engine::GetEngine( )->GetUserInputHandle( )->GetKeyState( GLFW_KEY_P ).isPressed )
                {
                    TileSpriteSetShared->PrintTileList( );
                }

                if ( Engine::GetEngine( )->GetUserInputHandle( )->GetKeyState( GLFW_KEY_DELETE ).isPressed )
                {
                    for ( int i = 0; i < 910; ++i )
                    {
                        TileSpriteSetShared->AddTile( TmpMap[ TileSpriteSetShared->GetTileCount( ) ] );
                        TileSpriteSetShared->Advance( );
                    }
                    DidMove = true;
                }

                if ( DidMove )
                {
                    m_CameraLerp              = 0;
                    const auto CameraLocation = m_Camera->GetCoordinate( );
                    m_CameraStart             = glm::vec2 { CameraLocation.X, CameraLocation.Y };
                    const auto TileTransform  = TileSpriteSetShared->GetCurrentTileTransform( );
                    m_CameraEnd               = TileTransform;
                }
            } else
            {
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

    for ( const auto& Tile : TmpMap )
    {
        Map->AddTile( Tile );
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
    CameraLocked->SetScale( 1 / 1.5F );
    CameraLocked->UpdateProjectionMatrix( );

    CameraLocked->RegisterAsDefaultCamera( );
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
    FBSpLocked->SetupSprite( );

    m_IBSp = m_ActivePlayerSprite = AddConcept<SpriteSquareTexture>( 512, 512 );
    auto IBSpLocked               = m_IBSp.lock( );

    IBSpLocked->SetOrigin( 512 / 2, 512 / 2 );
    IBSpLocked->SetShader( m_SpriteShader );
    IBSpLocked->SetTexturePath( "Assets/Texture/Player/IceBall.png" );
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
        const auto SpriteSize     = 512;
        m_ExplosionSpriteTemplate = std::make_unique<SpriteSquareAnimatedTexture>( SpriteSize, SpriteSize );

        m_ExplosionSpriteTemplate->SetOrigin( SpriteSize / 2, SpriteSize / 2 );
        m_ExplosionSpriteTemplate->SetShader( m_SpriteShader );
        m_ExplosionSpriteTemplate->SetTexturePath( "Assets/Texture/explosion.png" );

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
    m_ToleranceBar->SetupSprite( );
}
