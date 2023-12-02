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
#include <Engine/Core/Graphic/Camera/PureConceptOrthoCamera.hpp>
#include <Engine/Core/Graphic/Sprites/SpriteSquareAnimatedTexture.hpp>
#include <Engine/Core/Input/UserInput.hpp>
#include <Engine/Core/Graphic/Canvas/Canvas.hpp>

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
    { 45, 7943, true },
    { 315, 8018, true },
    { 90, 8093, true },
    { 270, 8243, true },
    { 90, 8393, true },
    { 270, 8543, true },
    { 90, 8693, true },
    { 315, 8843, true },
    { 45, 8918, true },
    { 270, 8993, true },
    { 90, 9143, true },
    { 315, 9293, true },
    { 45, 9368, true },
    { 270, 9443, true },
    { 180, 9593, true },
    { 180, 9893 },
    { 180, 10193 },
    { 45, 10493 },
    { 135, 10568 },
    { 180, 10793 },
    { 270, 11093, true },
    { 90, 11243, true },
    { 90, 11393 },
    { 270, 11543, true },
    { 270, 11693 },
    { 180, 11843, true },
    { 90, 12143 },
    { 180, 12293 },
    { 90, 12593 },
    { 90, 12743 },
    { 270, 12893, true },
    { 270, 13043 },
    { 180, 13193, true },
    { 90, 13493 },
    { 270, 13643, true },
    { 270, 13793 },
    { 90, 13943, true },
    { 90, 14093 },
    { 180, 14243 },
    { 270, 14543, true },
    { 180, 14693 },
    { 270, 14993 },
    { 270, 15143 },
    { 90, 15293, true },
    { 90, 15443 },
    { 180, 15593 },
    { 90, 15893 },
    { 270, 16043, true },
    { 90, 16193, true },
    { 90, 16343 },
    { 270, 16493, true },
    { 180, 16643 },
    { 90, 16943, true },
    { 270, 17093, true },
    { 270, 17243 },
    { 90, 17393, true },
    { 270, 17543, true },
    { 270, 17693 },
    { 180, 17843, true },
    { 90, 18143 },
    { 270, 18293, true },
    { 90, 18443, true },
    { 90, 18593 },
    { 270, 18743, true },
    { 90, 18893, true },
    { 180, 19043 },
    { 90, 19343 },
    { 270, 19493, true },
    { 90, 19643, true },
    { 270, 19793, true },
    { 270, 19943 },
    { 90, 20093, true },
    { 270, 20243, true },
    { 270, 20393 },
    { 45, 20543, true },
    { 315, 20618, true },
    { 90, 20693, true },
    { 270, 20843, true },
    { 45, 20993, true },
    { 315, 21068, true },
    { 90, 21143, true },
    { 270, 21293, true },
    { 90, 21443, true },
    { 270, 21593, true },
    { 45, 21743, true },
    { 315, 21818, true },
    { 90, 21893, true },
    { 270, 22043, true },
    { 45, 22193, true },
    { 315, 22268, true },
    { 45, 22343, true },
    { 315, 22418, true },
    { 90, 22493, true },
    { 90, 22643 },
    { 270, 22793, true },
    { 45, 22943, true },
    { 315, 23018, true },
    { 90, 23093, true },
    { 90, 23243 },
    { 315, 23393, true },
    { 45, 23468, true },
    { 315, 23543, true },
    { 45, 23618, true },
    { 270, 23693, true },
    { 90, 23843, true },
    { 90, 23993 },
    { 315, 24143, true },
    { 45, 24218, true },
    { 270, 24293, true },
    { 270, 24443 },
    { 45, 24593, true },
    { 315, 24668, true },
    { 90, 24743, true },
    { 315, 24893, true },
    { 45, 24968, true },
    { 270, 25043, true },
    { 90, 25193, true },
    { 315, 25343, true },
    { 45, 25418, true },
    { 270, 25493, true },
    { 270, 25643 },
    { 45, 25793, true },
    { 315, 25868, true },
    { 45, 25943, true },
    { 315, 26018, true },
    { 90, 26093, true },
    { 270, 26243, true },
    { 45, 26393, true },
    { 315, 26468, true },
    { 90, 26543, true },
    { 270, 26693, true },
    { 45, 26843, true },
    { 315, 26918, true },
    { 45, 26993, true },
    { 315, 27068, true },
    { 90, 27143, true },
    { 90, 27293 },
    { 270, 27443, true },
    { 270, 27593 },
    { 45, 27743, true },
    { 315, 27818, true },
    { 90, 27893, true },
    { 270, 28043, true },
    { 45, 28193, true },
    { 315, 28268, true },
    { 90, 28343, true },
    { 90, 28493 },
    { 270, 28643, true },
    { 45, 28793, true },
    { 315, 28868, true },
    { 45, 28943, true },
    { 315, 29018, true },
    { 90, 29093, true },
    { 270, 29243, true },
    { 180, 29393, true },
    { 270, 29693, true },
    { 90, 29843, true },
    { 270, 29993, true },
    { 45, 30143, true },
    { 315, 30218, true },
    { 90, 30293, true },
    { 270, 30443, true },
    { 135, 30593, true },
    { 45, 30818 },
    { 270, 30893, true },
    { 270, 31043 },
    { 90, 31193, true },
    { 315, 31343, true },
    { 45, 31418, true },
    { 270, 31493, true },
    { 270, 31643 },
    { 90, 31793, true },
    { 315, 31943, true },
    { 45, 32018, true },
    { 270, 32093, true },
    { 270, 32243 },
    { 90, 32393, true },
    { 315, 32543, true },
    { 45, 32618, true },
    { 270, 32693, true },
    { 90, 32843, true },
    { 135, 32993 },
    { 45, 33218 },
    { 270, 33293, true },
    { 270, 33443 },
    { 90, 33593, true },
    { 315, 33743, true },
    { 45, 33818, true },
    { 270, 33893, true },
    { 90, 34043, true },
    { 315, 34193, true },
    { 45, 34268, true },
    { 315, 34343, true },
    { 45, 34418, true },
    { 270, 34493, true },
    { 90, 34643, true },
    { 270, 34793, true },
    { 45, 34943, true },
    { 315, 35018, true },
    { 90, 35093, true },
    { 90, 35243 },
    { 315, 35393, true },
    { 45, 35468, true },
    { 315, 35543, true },
    { 45, 35618, true },
    { 270, 35693, true },
    { 90, 35843, true },
    { 315, 35993, true },
    { 45, 36068, true },
    { 315, 36143, true },
    { 45, 36218, true },
    { 315, 36293, true },
    { 45, 36368, true },
    { 270, 36443, true },
    { 45, 36593, true },
    { 315, 36668, true },
    { 45, 36743, true },
    { 315, 36818, true },
    { 45, 36893, true },
    { 315, 36968, true },
    { 90, 37043, true },
    { 90, 37193 },
    { 315, 37343, true },
    { 45, 37418, true },
    { 315, 37493, true },
    { 45, 37568, true },
    { 315, 37643, true },
    { 45, 37718, true },
    { 315, 37793, true },
    { 45, 37868, true },
    { 315, 37943, true },
    { 45, 38018, true },
    { 315, 38093, true },
    { 45, 38168, true },
    { 315, 38243, true },
    { 45, 38318, true },
    { 270, 38393, true },
    { 90, 38543, true },
    { 90, 38693 },
    { 270, 38843, true },
    { 180, 38993, true },
    { 270, 39293, true },
    { 90, 39443, true },
    { 180, 39593 },
    { 180, 39893 },
    { 180, 40193 },
    { 180, 40493 },
    { 180, 40793 },
    { 180, 41093 },
    { 180, 41393 },
    { 270, 41693, true },
    { 270, 41843 },
    { 180, 41993, true },
    { 180, 42293 },
    { 180, 42593 },
    { 180, 42893 },
    { 90, 43193 },
    { 315, 43343, true },
    { 45, 43418, true },
    { 270, 43493, true },
    { 90, 43643, true },
    { 315, 43793, true },
    { 45, 43868, true },
    { 315, 43943, true },
    { 45, 44018, true },
    { 270, 44093, true },
    { 90, 44243, true },
    { 180, 44393 },
    { 180, 44693 },
    { 180, 44993 },
    { 180, 45293 },
    { 180, 45593 },
    { 180, 45893 },
    { 180, 46193 },
    { 270, 46493, true },
    { 270, 46643 },
    { 180, 46793 },
    { 180, 47093 },
    { 180, 47393 },
    { 180, 47693 },
    { 90, 47993, true },
    { 315, 48143, true },
    { 45, 48218, true },
    { 270, 48293, true },
    { 90, 48443, true },
    { 315, 48593, true },
    { 45, 48668, true },
    { 315, 48743, true },
    { 45, 48818, true },
    { 315, 48893, true },
    { 45, 48968, true },
    { 270, 49043, true },
    { 135, 49193, true },
    { 45, 49418 },
    { 270, 49493, true },
    { 90, 49643, true },
    { 315, 49793, true },
    { 90, 49868, true },
    { 315, 50018, true },
    { 90, 50093, true },
    { 90, 50243 },
    { 315, 50393, true },
    { 45, 50468, true },
    { 270, 50543, true },
    { 45, 50693, true },
    { 315, 50768, true },
    { 90, 50843, true },
    { 315, 50993, true },
    { 45, 51068, true },
    { 315, 51143, true },
    { 45, 51218, true },
    { 270, 51293, true },
    { 90, 51443, true },
    { 270, 51593, true },
    { 45, 51743, true },
    { 315, 51818, true },
    { 90, 51893, true },
    { 90, 52043 },
    { 315, 52193, true },
    { 45, 52268, true },
    { 315, 52343, true },
    { 45, 52418, true },
    { 270, 52493, true },
    { 90, 52643, true },
    { 270, 52793, true },
    { 45, 52943, true },
    { 315, 53018, true },
    { 90, 53093, true },
    { 270, 53243, true },
    { 45, 53393, true },
    { 315, 53468, true },
    { 45, 53543, true },
    { 315, 53618, true },
    { 45, 53693, true },
    { 315, 53768, true },
    { 90, 53843, true },
    { 225, 53993, true },
    { 45, 54218, true },
    { 270, 54293, true },
    { 90, 54443, true },
    { 315, 54593, true },
    { 90, 54668, true },
    { 315, 54818, true },
    { 90, 54893, true },
    { 270, 55043, true },
    { 45, 55193, true },
    { 315, 55268, true },
    { 90, 55343, true },
    { 315, 55493, true },
    { 45, 55568, true },
    { 270, 55643, true },
    { 60, 55793, true },
    { 300, 55893, true },
    { 60, 55993, true },
    { 300, 56093, true },
    { 60, 56193, true },
    { 300, 56293, true },
    { 90, 56393, true },
    { 315, 56543, true },
    { 45, 56618, true },
    { 270, 56693, true },
    { 90, 56843, true },
    { 315, 56993, true },
    { 45, 57068, true },
    { 315, 57143, true },
    { 45, 57218, true },
    { 270, 57293, true },
    { 90, 57443, true },
    { 90, 57593 },
    { 315, 57743, true },
    { 45, 57818, true },
    { 270, 57893, true },
    { 90, 58043, true },
    { 315, 58193, true },
    { 45, 58268, true },
    { 270, 58343, true },
    { 30, 58493, true },
    { 330, 58543, true },
    { 30, 58593, true },
    { 270, 58643, true },
    { 180, 58793, true },
    { 270, 59093, true },
    { 90, 59243, true },
    { 270, 59393, true },
    { 45, 59543, true },
    { 315, 59618, true },
    { 90, 59693, true },
    { 270, 59843, true },
    { 90, 59993, true },
    { 315, 60143, true },
    { 45, 60218, true },
    { 270, 60293, true },
    { 270, 60443 },
    { 45, 60593, true },
    { 315, 60668, true },
    { 45, 60743, true },
    { 315, 60818, true },
    { 90, 60893, true },
    { 270, 61043, true },
    { 270, 61193 },
    { 90, 61343, true },
    { 90, 61493 },
    { 315, 61643, true },
    { 45, 61718, true },
    { 270, 61793, true },
    { 45, 61943, true },
    { 315, 62018, true },
    { 45, 62093, true },
    { 315, 62168, true },
    { 90, 62243, true },
    { 180, 62393 },
    { 60, 62693 },
    { 300, 62793, true },
    { 60, 62893, true },
    { 300, 62993, true },
    { 60, 63093, true },
    { 300, 63193, true },
    { 60, 63293, true },
    { 300, 63393, true },
    { 60, 63493, true },
    { 240, 63593, true },
    { 60, 63793, true },
    { 120, 63893 },
    { 300, 64093, true },
    { 120, 64193, true },
    { 300, 64393, true },
    { 120, 64493, true },
    { 300, 64693, true },
    { 120, 64793, true },
    { 60, 64993 },
    { 300, 65093, true },
    { 60, 65193, true },
    { 300, 65293, true },
    { 120, 65393, true },
    { 60, 65593 },
    { 300, 65693, true },
    { 60, 65793, true },
    { 300, 65893, true },
    { 120, 65993, true },
    { 60, 66193 },
    { 240, 66293, true },
    { 60, 66493, true },
    { 240, 66593, true },
    { 300, 66793 },
    { 120, 66893, true },
    { 300, 67093, true },
    { 120, 67193, true },
    { 300, 67393, true },
    { 60, 67493, true },
    { 300, 67593, true },
    { 60, 67693, true },
    { 240, 67793, true },
    { 300, 67993 },
    { 180, 68093, true },
    { 120, 68393 },
    { 60, 68593 },
    { 240, 68693, true },
    { 300, 68893 },
    { 120, 68993, true },
    { 300, 69193, true },
    { 120, 69293, true },
    { 60, 69493 },
    { 240, 69593, true },
    { 60, 69793, true },
    { 300, 69893, true },
    { 60, 69993, true },
    { 300, 70093, true },
    { 120, 70193, true },
    { 300, 70393, true },
    { 60, 70493, true },
    { 300, 70593, true },
    { 60, 70693, true },
    { 240, 70793, true },
    { 300, 70993 },
    { 120, 71093, true },
    { 300, 71293, true },
    { 120, 71393, true },
    { 300, 71593, true },
    { 120, 71693, true },
    { 60, 71893 },
    { 240, 71993, true },
    { 60, 72193, true },
    { 300, 72293, true },
    { 60, 72393, true },
    { 300, 72493, true },
    { 90, 72593, true },
    { 180, 72743 },
    { 180, 73043 },
    { 45, 73343 },
    { 315, 73418, true },
    { 90, 73493, true },
    { 270, 73643, true },
    { 45, 73793, true },
    { 315, 73868, true },
    { 90, 73943, true },
    { 90, 74093 },
    { 270, 74243, true },
    { 90, 74393, true },
    { 315, 74543, true },
    { 45, 74618, true },
    { 270, 74693, true },
    { 90, 74843, true },
    { 315, 74993, true },
    { 45, 75068, true },
    { 315, 75143, true },
    { 45, 75218, true },
    { 270, 75293, true },
    { 180, 75443, true },
    { 315, 75743, true },
    { 45, 75818, true },
    { 270, 75893, true },
    { 90, 76043, true },
    { 315, 76193, true },
    { 45, 76268, true },
    { 315, 76343, true },
    { 45, 76418, true },
    { 270, 76493, true },
    { 90, 76643, true },
    { 90, 76793 },
    { 315, 76943, true },
    { 45, 77018, true },
    { 270, 77093, true },
    { 90, 77243, true },
    { 315, 77393, true },
    { 45, 77468, true },
    { 315, 77543, true },
    { 45, 77618, true },
    { 270, 77693, true },
    { 180, 77843, true },
    { 315, 78143, true },
    { 45, 78218, true },
    { 270, 78293, true },
    { 90, 78443, true },
    { 315, 78593, true },
    { 45, 78668, true },
    { 315, 78743, true },
    { 45, 78818, true },
    { 270, 78893, true },
    { 90, 79043, true },
    { 315, 79193, true },
    { 45, 79268, true },
    { 270, 79343, true },
    { 270, 79493 },
    { 45, 79643, true },
    { 315, 79718, true },
    { 45, 79793, true },
    { 315, 79868, true },
    { 90, 79943, true },
    { 270, 80093, true },
    { 90, 80243, true },
    { 90, 80393 },
    { 315, 80543, true },
    { 45, 80618, true },
    { 270, 80693, true },
    { 90, 80843, true },
    { 315, 80993, true },
    { 45, 81068, true },
    { 270, 81143, true },
    { 270, 81293 },
    { 45, 81443, true },
    { 315, 81518, true },
    { 45, 81593, true },
    { 315, 81668, true },
    { 45, 81743, true },
    { 315, 81818, true },
    { 90, 81893, true },
    { 90, 82043 },
    { 180, 82193 },
    { 90, 82493 },
    { 270, 82643, true },
    { 225, 82793 },
    { 45, 83018, true },
    { 270, 83093, true },
    { 45, 83243, true },
    { 315, 83318, true },
    { 135, 83393, true },
    { 45, 83618 },
    { 270, 83693, true },
    { 270, 83843 },
    { 90, 83993, true },
    { 315, 84143, true },
    { 45, 84218, true },
    { 270, 84293, true },
    { 270, 84443 },
    { 90, 84593, true },
    { 315, 84743, true },
    { 90, 84818, true },
    { 315, 84968, true },
    { 90, 85043, true },
    { 135, 85193 },
    { 315, 85418, true },
    { 90, 85493, true },
    { 315, 85643, true },
    { 45, 85718, true },
    { 225, 85793, true },
    { 45, 86018, true },
    { 270, 86093, true },
    { 270, 86243 },
    { 90, 86393, true },
    { 315, 86543, true },
    { 45, 86618, true },
    { 270, 86693, true },
    { 270, 86843 },
    { 45, 86993, true },
    { 315, 87068, true },
    { 45, 87143, true },
    { 315, 87218, true },
    { 90, 87293, true },
    { 90, 87443 },
    { 315, 87593, true },
    { 45, 87668, true },
    { 315, 87743, true },
    { 45, 87818, true },
    { 270, 87893, true },
    { 270, 88043 },
    { 45, 88193, true },
    { 315, 88268, true },
    { 45, 88343, true },
    { 315, 88418, true },
    { 90, 88493, true },
    { 270, 88643, true },
    { 45, 88793, true },
    { 315, 88868, true },
    { 45, 88943, true },
    { 315, 89018, true },
    { 90, 89093, true },
    { 315, 89243, true },
    { 45, 89318, true },
    { 315, 89393, true },
    { 45, 89468, true },
    { 315, 89543, true },
    { 45, 89618, true },
    { 315, 89693, true },
    { 45, 89768, true },
    { 270, 89843, true },
    { 45, 89993, true },
    { 315, 90068, true },
    { 45, 90143, true },
    { 315, 90218, true },
    { 45, 90293, true },
    { 315, 90368, true },
    { 45, 90443, true },
    { 315, 90518, true },
    { 45, 90593, true },
    { 315, 90668, true },
    { 45, 90743, true },
    { 315, 90818, true },
    { 45, 90893, true },
    { 315, 90968, true },
    { 45, 91043, true },
    { 315, 91118, true },
    { 120, 91193, true },
    { 60, 91393 },
    { 240, 91493, true },
    { 60, 91693, true },
    { 240, 91793, true },
    { 60, 91993, true },
    { 240, 92093, true },
    { 300, 92293 },
    { 180, 92393, true },
    { 180, 92693 },
    { 300, 92993, true },
    { 60, 93093, true },
    { 300, 93193, true },
    { 180, 93293, true },
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
    { 270, 97193, true },
    { 45, 97343, true },
    { 315, 97418, true },
    { 90, 97493, true },
    { 270, 97643, true },
    { 45, 97793, true },
    { 315, 97868, true },
    { 45, 97943, true },
    { 315, 98018, true },
    { 90, 98093, true },
    { 270, 98243, true },
    { 180, 98393, true },
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
    { 45, 102743, true },
    { 315, 102818, true },
    { 45, 102893, true },
    { 315, 102968, true },
    { 90, 103043, true },
    { 225, 103193, true },
    { 45, 103418, true },
    { 270, 103493, true },
    { 90, 103643, true },
    { 315, 103793, true },
    { 90, 103868, true },
    { 315, 104018, true },
    { 90, 104093, true },
    { 90, 104243 },
    { 315, 104393, true },
    { 45, 104468, true },
    { 270, 104543, true },
    { 45, 104693, true },
    { 315, 104768, true },
    { 90, 104843, true },
    { 315, 104993, true },
    { 45, 105068, true },
    { 315, 105143, true },
    { 45, 105218, true },
    { 270, 105293, true },
    { 90, 105443, true },
    { 270, 105593, true },
    { 45, 105743, true },
    { 315, 105818, true },
    { 90, 105893, true },
    { 270, 106043, true },
    { 45, 106193, true },
    { 315, 106268, true },
    { 45, 106343, true },
    { 315, 106418, true },
    { 90, 106493, true },
    { 270, 106643, true },
    { 90, 106793, true },
    { 315, 106943, true },
    { 45, 107018, true },
    { 270, 107093, true },
    { 90, 107243, true },
    { 315, 107393, true },
    { 45, 107468, true },
    { 315, 107543, true },
    { 45, 107618, true },
    { 315, 107693, true },
    { 45, 107768, true },
    { 270, 107843, true },
    { 135, 107993, true },
    { 45, 108218 },
    { 270, 108293, true },
    { 90, 108443, true },
    { 315, 108593, true },
    { 90, 108668, true },
    { 315, 108818, true },
    { 90, 108893, true },
    { 90, 109043 },
    { 315, 109193, true },
    { 45, 109268, true },
    { 270, 109343, true },
    { 45, 109493, true },
    { 315, 109568, true },
    { 90, 109643, true },
    { 300, 109793, true },
    { 60, 109893, true },
    { 300, 109993, true },
    { 60, 110093, true },
    { 300, 110193, true },
    { 60, 110293, true },
    { 270, 110393, true },
    { 45, 110543, true },
    { 315, 110618, true },
    { 90, 110693, true },
    { 90, 110843 },
    { 315, 110993, true },
    { 45, 111068, true },
    { 315, 111143, true },
    { 45, 111218, true },
    { 270, 111293, true },
    { 90, 111443, true },
    { 270, 111593, true },
    { 45, 111743, true },
    { 315, 111818, true },
    { 90, 111893, true },
    { 270, 112043, true },
    { 45, 112193, true },
    { 315, 112268, true },
    { 90, 112343, true },
    { 330, 112493, true },
    { 30, 112543, true },
    { 330, 112593, true },
    { 90, 112643, true },
    { 180, 112793, true },
    { 270, 113093 },
    { 270, 113243 },
    { 90, 113393, true },
    { 315, 113543, true },
    { 45, 113618, true },
    { 270, 113693, true },
    { 90, 113843, true },
    { 270, 113993, true },
    { 45, 114143, true },
    { 315, 114218, true },
    { 90, 114293, true },
    { 270, 114443, true },
    { 45, 114593, true },
    { 315, 114668, true },
    { 45, 114743, true },
    { 315, 114818, true },
    { 90, 114893, true },
    { 270, 115043, true },
    { 180, 115193, true },
    { 90, 115493 },
    { 315, 115643, true },
    { 45, 115718, true },
    { 270, 115793, true },
    { 45, 115943, true },
    { 315, 116018, true },
    { 45, 116093, true },
    { 315, 116168, true },
    { 90, 116243, true },
    { 270, 116393, true },
    { 270, 116543 },
    { 45, 116693, true },
    { 315, 116768, true },
    { 90, 116843, true },
    { 300, 116993, true },
    { 60, 117093, true },
    { 300, 117193, true },
    { 60, 117293, true },
    { 300, 117393, true },
    { 60, 117493, true },
    { 270, 117593, true },
    { 45, 117743, true },
    { 315, 117818, true },
    { 90, 117893, true },
    { 315, 118043, true },
    { 45, 118118, true },
    { 315, 118193, true },
    { 45, 118268, true },
    { 315, 118343, true },
    { 45, 118418, true },
    { 270, 118493, true },
    { 90, 118643, true },
    { 315, 118793, true },
    { 45, 118868, true },
    { 315, 118943, true },
    { 45, 119018, true },
    { 270, 119093, true },
    { 45, 119243, true },
    { 315, 119318, true },
    { 45, 119393, true },
    { 315, 119468, true },
    { 45, 119543, true },
    { 315, 119618, true },
    { 45, 119693, true },
    { 315, 119768, true },
    { 90, 119843, true },
    { 315, 119993, true },
    { 45, 120068, true },
    { 315, 120143, true },
    { 45, 120218, true },
    { 315, 120293, true },
    { 45, 120368, true },
    { 315, 120443, true },
    { 45, 120518, true },
    { 315, 120593, true },
    { 45, 120668, true },
    { 315, 120743, true },
    { 45, 120818, true },
    { 315, 120893, true },
    { 45, 120968, true },
    { 315, 121043, true },
    { 45, 121118, true },
    { 360, 121193, true },
    { 360, 121793, true, 200 / 3.F },
    { 360, 123593, true, 200 },
    { 360, 124193, true },
    { 360, 124793, true },
    { 360, 125393, true },
    { 360, 125993, true },
    { 360, 126593, true },
    { 180, 127193, true },
    { 180, 127493, true },
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
    { 90, 133493, true },
    { 270, 133643, true },
    { 90, 133793, true },
    { 270, 133943, true },
    { 270, 134093 },
    { 90, 134243, true },
    { 270, 134393, true },
    { 270, 134543 },
    { 90, 134693, true },
    { 270, 134843, true },
    { 45, 134993, true },
    { 315, 135068, true },
    { 45, 135143, true },
    { 315, 135218, true },
    { 180, 135293, true },
    { 225, 135593, true },
    { 45, 135818, true },
    { 270, 135893, true },
    { 90, 136043, true },
    { 315, 136193, true },
    { 90, 136268, true },
    { 315, 136418, true },
    { 90, 136493, true },
    { 90, 136643 },
    { 315, 136793, true },
    { 45, 136868, true },
    { 270, 136943, true },
    { 45, 137093, true },
    { 315, 137168, true },
    { 90, 137243, true },
    { 315, 137393, true },
    { 45, 137468, true },
    { 315, 137543, true },
    { 45, 137618, true },
    { 270, 137693, true },
    { 90, 137843, true },
    { 270, 137993, true },
    { 45, 138143, true },
    { 315, 138218, true },
    { 90, 138293, true },
    { 270, 138443, true },
    { 45, 138593, true },
    { 315, 138668, true },
    { 45, 138743, true },
    { 315, 138818, true },
    { 90, 138893, true },
    { 270, 139043, true },
    { 90, 139193, true },
    { 315, 139343, true },
    { 45, 139418, true },
    { 270, 139493, true },
    { 90, 139643, true },
    { 315, 139793, true },
    { 45, 139868, true },
    { 315, 139943, true },
    { 45, 140018, true },
    { 315, 140093, true },
    { 45, 140168, true },
    { 270, 140243, true },
    { 135, 140393, true },
    { 315, 140618, true },
    { 90, 140693, true },
    { 90, 140843 },
    { 315, 140993, true },
    { 90, 141068, true },
    { 315, 141218, true },
    { 90, 141293, true },
    { 90, 141443 },
    { 315, 141593, true },
    { 45, 141668, true },
    { 270, 141743, true },
    { 45, 141893, true },
    { 315, 141968, true },
    { 90, 142043, true },
    { 315, 142193, true },
    { 45, 142268, true },
    { 315, 142343, true },
    { 45, 142418, true },
    { 315, 142493, true },
    { 45, 142568, true },
    { 315, 142643, true },
    { 45, 142718, true },
    { 270, 142793, true },
    { 45, 142943, true },
    { 315, 143018, true },
    { 90, 143093, true },
    { 90, 143243 },
    { 315, 143393, true },
    { 45, 143468, true },
    { 315, 143543, true },
    { 45, 143618, true },
    { 270, 143693, true },
    { 90, 143843, true },
    { 90, 143993 },
    { 315, 144143, true },
    { 45, 144218, true },
    { 270, 144293, true },
    { 90, 144443, true },
    { 315, 144593, true },
    { 45, 144668, true },
    { 315, 144743, true },
    { 45, 144818, true },
    { 330, 144893, true },
    { 30, 144943, true },
    { 330, 144993, true },
    { 90, 145043, true },
    { 180, 145193, true },
    { 180, 145493, true },
    { 180, 145793 },
    { 180, 146093 },
    { 180, 146393 },
    { 180, 146693 },
    { 180, 146993 },
    { 270, 147293, true },
    { 90, 147443, true },
    { 180, 147593 },
    { 180, 147893 },
    { 180, 148193 },
    { 180, 148493 },
    { 90, 148793 },
    { 270, 148943, true },
    { 90, 149093, true },
    { 270, 149243, true },
    { 270, 149393 },
    { 90, 149543, true },
    { 270, 149693, true },
    { 270, 149843 },
    { 135, 149993, true },
    { 45, 150218 },
    { 270, 150293, true },
    { 270, 150443 },
    { 135, 150593, true },
    { 45, 150818 },
    { 270, 150893, true },
    { 90, 151043, true },
    { 135, 151193 },
    { 315, 151418, true },
    { 90, 151493, true },
    { 90, 151643 },
    { 225, 151793, true },
    { 45, 152018, true },
    { 270, 152093, true },
    { 270, 152243 },
    { 135, 152393, true },
    { 45, 152618 },
    { 270, 152693, true },
    { 90, 152843, true },
    { 270, 152993, true },
    { 90, 153143, true },
    { 270, 153293, true },
    { 270, 153443 },
    { 180, 153593, true },
    { 45, 153893 },
    { 315, 153968, true },
    { 90, 154043, true },
    { 315, 154193, true },
    { 45, 154268, true },
    { 315, 154343, true },
    { 45, 154418, true },
    { 315, 154493, true },
    { 45, 154568, true },
    { 270, 154643, true },
    { 225, 154793 },
    { 45, 155018, true },
    { 270, 155093, true },
    { 270, 155243 },
    { 45, 155393, true },
    { 315, 155468, true },
    { 45, 155543, true },
    { 315, 155618, true },
    { 90, 155693, true },
    { 270, 155843, true },
    { 45, 155993, true },
    { 315, 156068, true },
    { 45, 156143, true },
    { 315, 156218, true },
    { 45, 156293, true },
    { 315, 156368, true },
    { 90, 156443, true },
    { 315, 156593, true },
    { 45, 156668, true },
    { 315, 156743, true },
    { 45, 156818, true },
    { 315, 156893, true },
    { 45, 156968, true },
    { 315, 157043, true },
    { 45, 157118, true },
    { 270, 157193, true },
    { 45, 157343, true },
    { 315, 157418, true },
    { 90, 157493, true },
    { 90, 157643 },
    { 315, 157793, true },
    { 45, 157868, true },
    { 315, 157943, true },
    { 45, 158018, true },
    { 270, 158093, true },
    { 90, 158243, true },
    { 315, 158393, true },
    { 45, 158468, true },
    { 270, 158543, true },
    { 45, 158693, true },
    { 315, 158768, true },
    { 90, 158843, true },
    { 315, 158993, true },
    { 45, 159068, true },
    { 315, 159143, true },
    { 45, 159218, true },
    { 315, 159293, true },
    { 45, 159368, true },
    { 270, 159443, true },
    { 90, 159593, true },
    { 315, 159743, true },
    { 45, 159818, true },
    { 270, 159893, true },
    { 45, 160043, true },
    { 315, 160118, true },
    { 45, 160193, true },
    { 315, 160268, true },
    { 45, 160343, true },
    { 315, 160418, true },
    { 90, 160493, true },
    { 90, 160643 },
    { 315, 160793, true },
    { 45, 160868, true },
    { 315, 160943, true },
    { 45, 161018, true },
    { 330, 161093, true },
    { 30, 161143, true },
    { 330, 161193, true },
    { 90, 161243, true },
    { 300, 161393, true },
    { 60, 161493, true },
    { 300, 161593, true },
    { 60, 161693, true },
    { 300, 161793, true },
    { 60, 161893, true },
    { 270, 161993, true },
    { 45, 162143, true },
    { 315, 162218, true },
    { 45, 162293, true },
    { 315, 162368, true },
    { 90, 162443, true },
    { 315, 162593, true },
    { 45, 162668, true },
    { 315, 162743, true },
    { 45, 162818, true },
    { 315, 162893, true },
    { 45, 162968, true },
    { 270, 163043, true },
    { 90, 163193, true },
    { 315, 163343, true },
    { 45, 163418, true },
    { 270, 163493, true },
    { 45, 163643, true },
    { 315, 163718, true },
    { 45, 163793, true },
    { 315, 163868, true },
    { 45, 163943, true },
    { 315, 164018, true },
    { 30, 164093, true },
    { 330, 164143, true },
    { 30, 164193, true },
    { 270, 164243, true },
    { 180, 164393, true },
    { 270, 164693, true },
    { 90, 164843, true },
    { 270, 164993, true },
    { 45, 165143, true },
    { 315, 165218, true },
    { 90, 165293, true },
    { 270, 165443, true },
    { 270, 165593 },
    { 90, 165743, true },
    { 270, 165893, true },
    { 90, 166043, true },
    { 315, 166193, true },
    { 45, 166268, true },
    { 315, 166343, true },
    { 45, 166418, true },
    { 270, 166493, true },
    { 90, 166643, true },
    { 270, 166793, true },
    { 90, 166943, true },
    { 90, 167093 },
    { 315, 167243, true },
    { 45, 167318, true },
    { 270, 167393, true },
    { 90, 167543, true },
    { 315, 167693, true },
    { 45, 167768, true },
    { 270, 167843, true },
    { 90, 167993, true },
    { 270, 168143, true },
    { 90, 168293, true },
    { 270, 168443, true },
    { 45, 168593, true },
    { 315, 168668, true },
    { 90, 168743, true },
    { 270, 168893, true },
    { 270, 169043 },
    { 180, 169193, true },
    { 90, 169493 },
    { 90, 169643 },
    { 270, 169793, true },
    { 90, 169943, true },
    { 180, 170093 },
    { 270, 170393, true },
    { 90, 170543, true },
    { 270, 170693, true },
    { 90, 170843, true },
    { 270, 170993, true },
    { 90, 171143, true },
    { 180, 171293 },
    { 270, 171593, true },
    { 90, 171743, true },
    { 270, 171893, true },
    { 45, 172043, true },
    { 315, 172118, true },
    { 90, 172193, true },
    { 270, 172343, true },
    { 45, 172493, true },
    { 315, 172568, true },
    { 90, 172643, true },
    { 180, 172793 },
    { 180, 173093 },
    { 180, 173393 },
    { 45, 173693 },
    { 360, 173768 },
};

}

class WhiteBackground : public ConceptRenderable
{
    DECLARE_CONCEPT( WhiteBackground, ConceptRenderable )
public:
    void
    Render( ) override
    {
        const auto* gl = Engine::GetEngine( )->GetGLContext( );
        GL_CHECK( Engine::GetEngine( )->MakeMainWindowCurrentContext( ) )
        gl->ClearColor( 1.F, 1.F, 1.F, 1.0f );
        gl->Clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    }
};
WhiteBackground::~WhiteBackground( ) { }

GameManager::GameManager( )
{
    spdlog::info( "GameManager concept constructor called" );

    AddConcept<WhiteBackground>( );

    {
        auto FixedCamera = AddConcept<PureConceptOrthoCamera>( );

        FixedCamera->SetScale( 1 / 1.5F );
        FixedCamera->UpdateCameraMatrix( );

        m_UICanvas = AddConcept<Canvas>( );
        m_UICanvas->SetRuntimeName( "UI Canvas" );
        m_UICanvas->SetCanvasCamera( FixedCamera );
    }

    SetupCamera( );
    LoadAudio( );

    m_TileSpriteSetRef = AddConcept<TileSpriteSet>( );

    SetupShader( );

    LoadPlayerSprites( );

    LoadTileSprites( { 360, 330, 315, 300, 270, 240, 225, 180, 135, 120, 90, 60, 45, 30, 0 } );
    {
        auto* Sp = m_TileSpriteSetRef.lock( )->RegisterReverseSprite( std::make_unique<SpriteSquareTexture>( 512, 512 ) );

        Sp->SetShader( m_SpriteShader );
        Sp->SetTexturePath( "Assets/Texture/Tile/reverse.png" );
        Sp->SetupSprite( );
    }

    LoadTileMap( );

    SetupExplosionSpriteTemplate( );
    LoadToleranceSprite( );

    {
        m_StartButton = m_UICanvas->AddConcept<RectButton>( -25, -12 );
        m_StartButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
        m_StartButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
        m_StartButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
        m_StartButton->SetText( "Start" );
        m_StartButton->SetPivot( 0.5F, 0.5F );
        m_StartButton->SetCoordinate( 0, 70 );
        m_StartButton->SetCallback( [ this ]( ) {
            m_Playing = true;
            m_MainAudioHandle.Resume( );

            m_StartButton->Destroy( );
            m_OffsetWizardButton->Destroy( );

            m_StartButton.reset( );
            m_OffsetWizardButton.reset( );
        } );
    }

    {
        m_OffsetWizardButton = m_UICanvas->AddConcept<RectButton>( -25, -12 );
        m_OffsetWizardButton->SetPressReactColor( glm::vec4 { 0.9, 0.9, 0.9, 1 } );
        m_OffsetWizardButton->SetDefaultColor( glm::vec4 { 0.3, 0.3, 0.3, 1 } );
        m_OffsetWizardButton->SetTextColor( glm::vec3 { 1, 1, 1 } );
        m_OffsetWizardButton->SetText( "Start Offset Wizard" );
        m_OffsetWizardButton->SetPivot( 0.5F, 0.5F );
        m_OffsetWizardButton->SetCoordinate( 0, -70 );
        m_OffsetWizardButton->SetCallback( [ this ]( ) {
            m_StartButton->SetEnabled( false );
            m_OffsetWizardButton->SetEnabled( false );

            m_IsCheckingDeviceDelay = true;
            m_DelayCheckingHandle   = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( m_DelayCheckingSoundSource, true );
        } );
    }

    m_InActivePlayerSprite->SetRotation( 0, 0, glm::radians( 180.f ) );

    m_UICanvas->MoveToLastAsSubConcept( );
    spdlog::info( "GameManager concept constructor returned" );
}

void
GameManager::Apply( )
{
    if ( m_IsCheckingDeviceDelay ) [[unlikely]]
    {
        ApplyOffsetWizard( );
    } else if ( m_Playing )
    {
        const auto DeltaSecond    = Engine::GetEngine( )->GetDeltaSecond( );
        const bool PlayerInteract = IsUserPrimaryInteract( );

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
                    m_Camera->UpdateCameraMatrix( );

                    m_CameraLerp = 1;
                } else
                {
                    const auto LerpCoordinate = glm::lerp( m_CameraStart, m_CameraEnd, m_CameraLerp );
                    m_Camera->SetCoordinate( LerpCoordinate.x, LerpCoordinate.y );
                    m_Camera->UpdateCameraMatrix( );
                }
            }

            if ( m_ManualMapping )
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
                        TmpMap[ TileSpriteSetShared->GetTileCount( ) + 1 ].ReverseDirection = !TmpMap[ TileSpriteSetShared->GetTileCount( ) + 1 ].ReverseDirection;
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
                    for ( int i = 0; i < 1205; ++i )
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
                    m_CameraStart             = glm::vec2 { CameraLocation.x, CameraLocation.y };
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
                    // Play note hit effect when device delay is low enough
                    if ( std::abs( m_UserDeviceOffsetMS ) <= 100 )
                    {
                        Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( m_NoteHitSfxSource );
                    }

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
                    m_CameraStart             = glm::vec2 { CameraLocation.x, CameraLocation.y };
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

    m_TileSpriteSetRef.lock( )->SetSpritesOrigin( glm::vec3 { 512 / 2, 512 / 2, 0 } );

    for ( auto Degree : Degrees )
    {
        AddDegreeTile( Degree );
    }
}

void
GameManager::LoadTileMap( )
{
    auto Map = m_TileSpriteSetRef.lock( );

    if ( m_ManualMapping )
    {
        Map->AddTile( TmpMap[ 0 ] );
        Map->AddTile( TmpMap[ 1 ] );

    } else
    {
        for ( const auto& Tile : TmpMap )
        {
            Map->AddTile( Tile );
        }
    }
}

void
GameManager::LoadAudio( )
{
    m_PlayingSpeed = 1;

    m_DelayCheckingSoundSource = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/Beats.ogg" );

    m_NoteHitSfxSource = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/NoteHit.wav" );

    auto* MAC         = Engine::GetEngine( )->GetAudioEngine( )->CreateAudioHandle( "Assets/Audio/Papipupipupipa.ogg" );
    m_MainAudioHandle = Engine::GetEngine( )->GetAudioEngine( )->PlayAudio( MAC, true, true );
    m_MainAudioHandle.SetSpeed( m_PlayingSpeed );
    m_MainAudioHandle.SetVolume( 0.2 );
    SetBPM( 200 * m_PlayingSpeed );
}

void
GameManager::SetupCamera( )
{
    m_Camera = AddConcept<PureConceptOrthoCamera>( );

    m_Camera->SetScale( 1 / 1.5F );
    m_Camera->UpdateCameraMatrix( );

    m_Camera->PushToCameraStack( );
}

void
GameManager::LoadPlayerSprites( )
{
    m_FBSp = m_InActivePlayerSprite = AddConcept<SpriteSquareTexture>( 512, 512 );

    auto FBSpLocked = m_FBSp.lock( );
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
    m_ActivePlayerIsFire = !m_ActivePlayerIsFire;
    std::swap( m_ActivePlayerSprite, m_InActivePlayerSprite );

    m_InActivePlayerSprite->SetOrigin( 512 / 2 - TileSpriteSet::TileDistance, 512 / 2 );
    m_ActivePlayerSprite->SetOrigin( 512 / 2, 512 / 2 );

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
    m_ToleranceBar = m_UICanvas->AddConcept<ToleranceBar>( 400, 30 );

    m_ToleranceBar->SetOrigin( 400 / 2, 30 / 2 );

    m_ToleranceBar->SetShader( m_SpriteShader );
    m_ToleranceBar->SetTexturePath( "Assets/Texture/tolerance.png" );
    m_ToleranceBar->SetupSprite( );
}

void
GameManager::ApplyOffsetWizard( )
{
    // Adjust the audio offset to the correct value
    (void) m_DelayCheckingHandle.GetCorrectedCurrentAudioOffset( );

    if ( IsUserPrimaryInteract( ) )
    {
        UpdateDeviceOffset( );
    }

    // End of offset wizard
    if ( m_DelayCheckingHandle.IsAudioEnded( ) )
    {
        spdlog::info( "End of offset wizard, final offset: {}ms", m_UserDeviceOffsetMS );
        m_IsCheckingDeviceDelay = false;

        m_StartButton->SetEnabled( true );
        m_OffsetWizardButton->SetEnabled( true );
    }
}
