#include "ColorExtern.h"


namespace core
{
namespace color
{


// CIE X function values
const Float CIE_X[CIE_SAMPLES_NUMBER] = {
    static_cast<Float>(0.0001299000f), static_cast<Float>(0.0001458470f), static_cast<Float>(0.0001638021f)
    , static_cast<Float>(0.0001840037f), static_cast<Float>(0.0002066902f), static_cast<Float>(0.0002321000f)
    , static_cast<Float>(0.0002607280f), static_cast<Float>(0.0002930750f), static_cast<Float>(0.0003293880f)
    , static_cast<Float>(0.0003699140f), static_cast<Float>(0.0004149000f), static_cast<Float>(0.0004641587f)
    , static_cast<Float>(0.0005189860f), static_cast<Float>(0.0005818540f), static_cast<Float>(0.0006552347f)
    , static_cast<Float>(0.0007416000f), static_cast<Float>(0.0008450296f), static_cast<Float>(0.0009645268f)
    , static_cast<Float>(0.001094949f), static_cast<Float>(0.001231154f), static_cast<Float>(0.001368000f)
    , static_cast<Float>(0.001502050f), static_cast<Float>(0.001642328f), static_cast<Float>(0.001802382f)
    , static_cast<Float>(0.001995757f), static_cast<Float>(0.002236000f), static_cast<Float>(0.002535385f)
    , static_cast<Float>(0.002892603f), static_cast<Float>(0.003300829f), static_cast<Float>(0.003753236f)
    , static_cast<Float>(0.004243000f), static_cast<Float>(0.004762389f), static_cast<Float>(0.005330048f)
    , static_cast<Float>(0.005978712f), static_cast<Float>(0.006741117f), static_cast<Float>(0.007650000f)
    , static_cast<Float>(0.008751373f), static_cast<Float>(0.01002888f), static_cast<Float>(0.01142170f)
    , static_cast<Float>(0.01286901f), static_cast<Float>(0.01431000f), static_cast<Float>(0.01570443f)
    , static_cast<Float>(0.01714744f), static_cast<Float>(0.01878122f), static_cast<Float>(0.02074801f)
    , static_cast<Float>(0.02319000f), static_cast<Float>(0.02620736f), static_cast<Float>(0.02978248f)
    , static_cast<Float>(0.03388092f), static_cast<Float>(0.03846824f), static_cast<Float>(0.04351000f)
    , static_cast<Float>(0.04899560f), static_cast<Float>(0.05502260f), static_cast<Float>(0.06171880f)
    , static_cast<Float>(0.06921200f), static_cast<Float>(0.07763000f), static_cast<Float>(0.08695811f)
    , static_cast<Float>(0.09717672f), static_cast<Float>(0.1084063f), static_cast<Float>(0.1207672f)
    , static_cast<Float>(0.1343800f), static_cast<Float>(0.1493582f), static_cast<Float>(0.1653957f)
    , static_cast<Float>(0.1819831f), static_cast<Float>(0.1986110f), static_cast<Float>(0.2147700f)
    , static_cast<Float>(0.2301868f), static_cast<Float>(0.2448797f), static_cast<Float>(0.2587773f)
    , static_cast<Float>(0.2718079f), static_cast<Float>(0.2839000f), static_cast<Float>(0.2949438f)
    , static_cast<Float>(0.3048965f), static_cast<Float>(0.3137873f), static_cast<Float>(0.3216454f)
    , static_cast<Float>(0.3285000f), static_cast<Float>(0.3343513f), static_cast<Float>(0.3392101f)
    , static_cast<Float>(0.3431213f), static_cast<Float>(0.3461296f), static_cast<Float>(0.3482800f)
    , static_cast<Float>(0.3495999f), static_cast<Float>(0.3501474f), static_cast<Float>(0.3500130f)
    , static_cast<Float>(0.3492870f), static_cast<Float>(0.3480600f), static_cast<Float>(0.3463733f)
    , static_cast<Float>(0.3442624f), static_cast<Float>(0.3418088f), static_cast<Float>(0.3390941f)
    , static_cast<Float>(0.3362000f), static_cast<Float>(0.3331977f), static_cast<Float>(0.3300411f)
    , static_cast<Float>(0.3266357f), static_cast<Float>(0.3228868f), static_cast<Float>(0.3187000f)
    , static_cast<Float>(0.3140251f), static_cast<Float>(0.3088840f), static_cast<Float>(0.3032904f)
    , static_cast<Float>(0.2972579f), static_cast<Float>(0.2908000f), static_cast<Float>(0.2839701f)
    , static_cast<Float>(0.2767214f), static_cast<Float>(0.2689178f), static_cast<Float>(0.2604227f)
    , static_cast<Float>(0.2511000f), static_cast<Float>(0.2408475f), static_cast<Float>(0.2298512f)
    , static_cast<Float>(0.2184072f), static_cast<Float>(0.2068115f), static_cast<Float>(0.1953600f)
    , static_cast<Float>(0.1842136f), static_cast<Float>(0.1733273f), static_cast<Float>(0.1626881f)
    , static_cast<Float>(0.1522833f), static_cast<Float>(0.1421000f), static_cast<Float>(0.1321786f)
    , static_cast<Float>(0.1225696f), static_cast<Float>(0.1132752f), static_cast<Float>(0.1042979f)
    , static_cast<Float>(0.09564000f), static_cast<Float>(0.08729955f), static_cast<Float>(0.07930804f)
    , static_cast<Float>(0.07171776f), static_cast<Float>(0.06458099f), static_cast<Float>(0.05795001f)
    , static_cast<Float>(0.05186211f), static_cast<Float>(0.04628152f), static_cast<Float>(0.04115088f)
    , static_cast<Float>(0.03641283f), static_cast<Float>(0.03201000f), static_cast<Float>(0.02791720f)
    , static_cast<Float>(0.02414440f), static_cast<Float>(0.02068700f), static_cast<Float>(0.01754040f)
    , static_cast<Float>(0.01470000f), static_cast<Float>(0.01216179f), static_cast<Float>(0.009919960f)
    , static_cast<Float>(0.007967240f), static_cast<Float>(0.006296346f), static_cast<Float>(0.004900000f)
    , static_cast<Float>(0.003777173f), static_cast<Float>(0.002945320f), static_cast<Float>(0.002424880f)
    , static_cast<Float>(0.002236293f), static_cast<Float>(0.002400000f), static_cast<Float>(0.002925520f)
    , static_cast<Float>(0.003836560f), static_cast<Float>(0.005174840f), static_cast<Float>(0.006982080f)
    , static_cast<Float>(0.009300000f), static_cast<Float>(0.01214949f), static_cast<Float>(0.01553588f)
    , static_cast<Float>(0.01947752f), static_cast<Float>(0.02399277f), static_cast<Float>(0.02910000f)
    , static_cast<Float>(0.03481485f), static_cast<Float>(0.04112016f), static_cast<Float>(0.04798504f)
    , static_cast<Float>(0.05537861f), static_cast<Float>(0.06327000f), static_cast<Float>(0.07163501f)
    , static_cast<Float>(0.08046224f), static_cast<Float>(0.08973996f), static_cast<Float>(0.09945645f)
    , static_cast<Float>(0.1096000f), static_cast<Float>(0.1201674f), static_cast<Float>(0.1311145f)
    , static_cast<Float>(0.1423679f), static_cast<Float>(0.1538542f), static_cast<Float>(0.1655000f)
    , static_cast<Float>(0.1772571f), static_cast<Float>(0.1891400f), static_cast<Float>(0.2011694f)
    , static_cast<Float>(0.2133658f), static_cast<Float>(0.2257499f), static_cast<Float>(0.2383209f)
    , static_cast<Float>(0.2510668f), static_cast<Float>(0.2639922f), static_cast<Float>(0.2771017f)
    , static_cast<Float>(0.2904000f), static_cast<Float>(0.3038912f), static_cast<Float>(0.3175726f)
    , static_cast<Float>(0.3314384f), static_cast<Float>(0.3454828f), static_cast<Float>(0.3597000f)
    , static_cast<Float>(0.3740839f), static_cast<Float>(0.3886396f), static_cast<Float>(0.4033784f)
    , static_cast<Float>(0.4183115f), static_cast<Float>(0.4334499f), static_cast<Float>(0.4487953f)
    , static_cast<Float>(0.4643360f), static_cast<Float>(0.4800640f), static_cast<Float>(0.4959713f)
    , static_cast<Float>(0.5120501f), static_cast<Float>(0.5282959f), static_cast<Float>(0.5446916f)
    , static_cast<Float>(0.5612094f), static_cast<Float>(0.5778215f), static_cast<Float>(0.5945000f)
    , static_cast<Float>(0.6112209f), static_cast<Float>(0.6279758f), static_cast<Float>(0.6447602f)
    , static_cast<Float>(0.6615697f), static_cast<Float>(0.6784000f), static_cast<Float>(0.6952392f)
    , static_cast<Float>(0.7120586f), static_cast<Float>(0.7288284f), static_cast<Float>(0.7455188f)
    , static_cast<Float>(0.7621000f), static_cast<Float>(0.7785432f), static_cast<Float>(0.7948256f)
    , static_cast<Float>(0.8109264f), static_cast<Float>(0.8268248f), static_cast<Float>(0.8425000f)
    , static_cast<Float>(0.8579325f), static_cast<Float>(0.8730816f), static_cast<Float>(0.8878944f)
    , static_cast<Float>(0.9023181f), static_cast<Float>(0.9163000f), static_cast<Float>(0.9297995f)
    , static_cast<Float>(0.9427984f), static_cast<Float>(0.9552776f), static_cast<Float>(0.9672179f)
    , static_cast<Float>(0.9786000f), static_cast<Float>(0.9893856f), static_cast<Float>(0.9995488f)
    , static_cast<Float>(1.0090892f), static_cast<Float>(1.0180064f), static_cast<Float>(1.0263000f)
    , static_cast<Float>(1.0339827f), static_cast<Float>(1.0409860f), static_cast<Float>(1.0471880f)
    , static_cast<Float>(1.0524667f), static_cast<Float>(1.0567000f), static_cast<Float>(1.0597944f)
    , static_cast<Float>(1.0617992f), static_cast<Float>(1.0628068f), static_cast<Float>(1.0629096f)
    , static_cast<Float>(1.0622000f), static_cast<Float>(1.0607352f), static_cast<Float>(1.0584436f)
    , static_cast<Float>(1.0552244f), static_cast<Float>(1.0509768f), static_cast<Float>(1.0456000f)
    , static_cast<Float>(1.0390369f), static_cast<Float>(1.0313608f), static_cast<Float>(1.0226662f)
    , static_cast<Float>(1.0130477f), static_cast<Float>(1.0026000f), static_cast<Float>(0.9913675f)
    , static_cast<Float>(0.9793314f), static_cast<Float>(0.9664916f), static_cast<Float>(0.9528479f)
    , static_cast<Float>(0.9384000f), static_cast<Float>(0.9231940f), static_cast<Float>(0.9072440f)
    , static_cast<Float>(0.8905020f), static_cast<Float>(0.8729200f), static_cast<Float>(0.8544499f)
    , static_cast<Float>(0.8350840f), static_cast<Float>(0.8149460f), static_cast<Float>(0.7941860f)
    , static_cast<Float>(0.7729540f), static_cast<Float>(0.7514000f), static_cast<Float>(0.7295836f)
    , static_cast<Float>(0.7075888f), static_cast<Float>(0.6856022f), static_cast<Float>(0.6638104f)
    , static_cast<Float>(0.6424000f), static_cast<Float>(0.6215149f), static_cast<Float>(0.6011138f)
    , static_cast<Float>(0.5811052f), static_cast<Float>(0.5613977f), static_cast<Float>(0.5419000f)
    , static_cast<Float>(0.5225995f), static_cast<Float>(0.5035464f), static_cast<Float>(0.4847436f)
    , static_cast<Float>(0.4661939f), static_cast<Float>(0.4479000f), static_cast<Float>(0.4298613f)
    , static_cast<Float>(0.4120980f), static_cast<Float>(0.3946440f), static_cast<Float>(0.3775333f)
    , static_cast<Float>(0.3608000f), static_cast<Float>(0.3444563f), static_cast<Float>(0.3285168f)
    , static_cast<Float>(0.3130192f), static_cast<Float>(0.2980011f), static_cast<Float>(0.2835000f)
    , static_cast<Float>(0.2695448f), static_cast<Float>(0.2561184f), static_cast<Float>(0.2431896f)
    , static_cast<Float>(0.2307272f), static_cast<Float>(0.2187000f), static_cast<Float>(0.2070971f)
    , static_cast<Float>(0.1959232f), static_cast<Float>(0.1851708f), static_cast<Float>(0.1748323f)
    , static_cast<Float>(0.1649000f), static_cast<Float>(0.1553667f), static_cast<Float>(0.1462300f)
    , static_cast<Float>(0.1374900f), static_cast<Float>(0.1291467f), static_cast<Float>(0.1212000f)
    , static_cast<Float>(0.1136397f), static_cast<Float>(0.1064650f), static_cast<Float>(0.09969044f)
    , static_cast<Float>(0.09333061f), static_cast<Float>(0.08740000f), static_cast<Float>(0.08190096f)
    , static_cast<Float>(0.07680428f), static_cast<Float>(0.07207712f), static_cast<Float>(0.06768664f)
    , static_cast<Float>(0.06360000f), static_cast<Float>(0.05980685f), static_cast<Float>(0.05628216f)
    , static_cast<Float>(0.05297104f), static_cast<Float>(0.04981861f), static_cast<Float>(0.04677000f)
    , static_cast<Float>(0.04378405f), static_cast<Float>(0.04087536f), static_cast<Float>(0.03807264f)
    , static_cast<Float>(0.03540461f), static_cast<Float>(0.03290000f), static_cast<Float>(0.03056419f)
    , static_cast<Float>(0.02838056f), static_cast<Float>(0.02634484f), static_cast<Float>(0.02445275f)
    , static_cast<Float>(0.02270000f), static_cast<Float>(0.02108429f), static_cast<Float>(0.01959988f)
    , static_cast<Float>(0.01823732f), static_cast<Float>(0.01698717f), static_cast<Float>(0.01584000f)
    , static_cast<Float>(0.01479064f), static_cast<Float>(0.01383132f), static_cast<Float>(0.01294868f)
    , static_cast<Float>(0.01212920f), static_cast<Float>(0.01135916f), static_cast<Float>(0.01062935f)
    , static_cast<Float>(0.009938846f), static_cast<Float>(0.009288422f), static_cast<Float>(0.008678854f)
    , static_cast<Float>(0.008110916f), static_cast<Float>(0.007582388f), static_cast<Float>(0.007088746f)
    , static_cast<Float>(0.006627313f), static_cast<Float>(0.006195408f), static_cast<Float>(0.005790346f)
    , static_cast<Float>(0.005409826f), static_cast<Float>(0.005052583f), static_cast<Float>(0.004717512f)
    , static_cast<Float>(0.004403507f), static_cast<Float>(0.004109457f), static_cast<Float>(0.003833913f)
    , static_cast<Float>(0.003575748f), static_cast<Float>(0.003334342f), static_cast<Float>(0.003109075f)
    , static_cast<Float>(0.002899327f), static_cast<Float>(0.002704348f), static_cast<Float>(0.002523020f)
    , static_cast<Float>(0.002354168f), static_cast<Float>(0.002196616f), static_cast<Float>(0.002049190f)
    , static_cast<Float>(0.001910960f), static_cast<Float>(0.001781438f), static_cast<Float>(0.001660110f)
    , static_cast<Float>(0.001546459f), static_cast<Float>(0.001439971f), static_cast<Float>(0.001340042f)
    , static_cast<Float>(0.001246275f), static_cast<Float>(0.001158471f), static_cast<Float>(0.001076430f)
    , static_cast<Float>(0.0009999493f), static_cast<Float>(0.0009287358f), static_cast<Float>(0.0008624332f)
    , static_cast<Float>(0.0008007503f), static_cast<Float>(0.0007433960f), static_cast<Float>(0.0006900786f)
    , static_cast<Float>(0.0006405156f), static_cast<Float>(0.0005945021f), static_cast<Float>(0.0005518646f)
    , static_cast<Float>(0.0005124290f), static_cast<Float>(0.0004760213f), static_cast<Float>(0.0004424536f)
    , static_cast<Float>(0.0004115117f), static_cast<Float>(0.0003829814f), static_cast<Float>(0.0003566491f)
    , static_cast<Float>(0.0003323011f), static_cast<Float>(0.0003097586f), static_cast<Float>(0.0002888871f)
    , static_cast<Float>(0.0002695394f), static_cast<Float>(0.0002515682f), static_cast<Float>(0.0002348261f)
    , static_cast<Float>(0.0002191710f), static_cast<Float>(0.0002045258f), static_cast<Float>(0.0001908405f)
    , static_cast<Float>(0.0001780654f), static_cast<Float>(0.0001661505f), static_cast<Float>(0.0001550236f)
    , static_cast<Float>(0.0001446219f), static_cast<Float>(0.0001349098f), static_cast<Float>(0.0001258520f)
    , static_cast<Float>(0.0001174130f), static_cast<Float>(0.0001095515f), static_cast<Float>(0.0001022245f)
    , static_cast<Float>(0.00009539445f), static_cast<Float>(0.00008902390f), static_cast<Float>(0.00008307527f)
    , static_cast<Float>(0.00007751269f), static_cast<Float>(0.00007231304f), static_cast<Float>(0.00006745778f)
    , static_cast<Float>(0.00006292844f), static_cast<Float>(0.00005870652f), static_cast<Float>(0.00005477028f)
    , static_cast<Float>(0.00005109918f), static_cast<Float>(0.00004767654f), static_cast<Float>(0.00004448567f)
    , static_cast<Float>(0.00004150994f), static_cast<Float>(0.00003873324f), static_cast<Float>(0.00003614203f)
    , static_cast<Float>(0.00003372352f), static_cast<Float>(0.00003146487f), static_cast<Float>(0.00002935326f)
    , static_cast<Float>(0.00002737573f), static_cast<Float>(0.00002552433f), static_cast<Float>(0.00002379376f)
    , static_cast<Float>(0.00002217870f), static_cast<Float>(0.00002067383f), static_cast<Float>(0.00001927226f)
    , static_cast<Float>(0.00001796640f), static_cast<Float>(0.00001674991f), static_cast<Float>(0.00001561648f)
    , static_cast<Float>(0.00001455977f), static_cast<Float>(0.00001357387f), static_cast<Float>(0.00001265436f)
    , static_cast<Float>(0.00001179723f), static_cast<Float>(0.00001099844f), static_cast<Float>(0.00001025398f)
    , static_cast<Float>(0.000009559646f), static_cast<Float>(0.000008912044f), static_cast<Float>(0.000008308358f)
    , static_cast<Float>(0.000007745769f), static_cast<Float>(0.000007221456f), static_cast<Float>(0.000006732475f)
    , static_cast<Float>(0.000006276423f), static_cast<Float>(0.000005851304f), static_cast<Float>(0.000005455118f)
    , static_cast<Float>(0.000005085868f), static_cast<Float>(0.000004741466f), static_cast<Float>(0.000004420236f)
    , static_cast<Float>(0.000004120783f), static_cast<Float>(0.000003841716f), static_cast<Float>(0.000003581652f)
    , static_cast<Float>(0.000003339127f), static_cast<Float>(0.000003112949f), static_cast<Float>(0.000002902121f)
    , static_cast<Float>(0.000002705645f), static_cast<Float>(0.000002522525f), static_cast<Float>(0.000002351726f)
    , static_cast<Float>(0.000002192415f), static_cast<Float>(0.000002043902f), static_cast<Float>(0.000001905497f)
    , static_cast<Float>(0.000001776509f), static_cast<Float>(0.000001656215f), static_cast<Float>(0.000001544022f)
    , static_cast<Float>(0.000001439440f), static_cast<Float>(0.000001341977f), static_cast<Float>(0.000001251141f)};

// CIE Y function values
const Float CIE_Y[CIE_SAMPLES_NUMBER] = {
    static_cast<Float>(0.000003917000f), static_cast<Float>(0.000004393581f), static_cast<Float>(0.000004929604f)
    , static_cast<Float>(0.000005532136f), static_cast<Float>(0.000006208245f), static_cast<Float>(0.000006965000f)
    , static_cast<Float>(0.000007813219f), static_cast<Float>(0.000008767336f), static_cast<Float>(0.000009839844f)
    , static_cast<Float>(0.00001104323f), static_cast<Float>(0.00001239000f), static_cast<Float>(0.00001388641f)
    , static_cast<Float>(0.00001555728f), static_cast<Float>(0.00001744296f), static_cast<Float>(0.00001958375f)
    , static_cast<Float>(0.00002202000f), static_cast<Float>(0.00002483965f), static_cast<Float>(0.00002804126f)
    , static_cast<Float>(0.00003153104f), static_cast<Float>(0.00003521521f), static_cast<Float>(0.00003900000f)
    , static_cast<Float>(0.00004282640f), static_cast<Float>(0.00004691460f), static_cast<Float>(0.00005158960f)
    , static_cast<Float>(0.00005717640f), static_cast<Float>(0.00006400000f), static_cast<Float>(0.00007234421f)
    , static_cast<Float>(0.00008221224f), static_cast<Float>(0.00009350816f), static_cast<Float>(0.0001061361f)
    , static_cast<Float>(0.0001200000f), static_cast<Float>(0.0001349840f), static_cast<Float>(0.0001514920f)
    , static_cast<Float>(0.0001702080f), static_cast<Float>(0.0001918160f), static_cast<Float>(0.0002170000f)
    , static_cast<Float>(0.0002469067f), static_cast<Float>(0.0002812400f), static_cast<Float>(0.0003185200f)
    , static_cast<Float>(0.0003572667f), static_cast<Float>(0.0003960000f), static_cast<Float>(0.0004337147f)
    , static_cast<Float>(0.0004730240f), static_cast<Float>(0.0005178760f), static_cast<Float>(0.0005722187f)
    , static_cast<Float>(0.0006400000f), static_cast<Float>(0.0007245600f), static_cast<Float>(0.0008255000f)
    , static_cast<Float>(0.0009411600f), static_cast<Float>(0.001069880f), static_cast<Float>(0.001210000f)
    , static_cast<Float>(0.001362091f), static_cast<Float>(0.001530752f), static_cast<Float>(0.001720368f)
    , static_cast<Float>(0.001935323f), static_cast<Float>(0.002180000f), static_cast<Float>(0.002454800f)
    , static_cast<Float>(0.002764000f), static_cast<Float>(0.003117800f), static_cast<Float>(0.003526400f)
    , static_cast<Float>(0.004000000f), static_cast<Float>(0.004546240f), static_cast<Float>(0.005159320f)
    , static_cast<Float>(0.005829280f), static_cast<Float>(0.006546160f), static_cast<Float>(0.007300000f)
    , static_cast<Float>(0.008086507f), static_cast<Float>(0.008908720f), static_cast<Float>(0.009767680f)
    , static_cast<Float>(0.01066443f), static_cast<Float>(0.01160000f), static_cast<Float>(0.01257317f)
    , static_cast<Float>(0.01358272f), static_cast<Float>(0.01462968f), static_cast<Float>(0.01571509f)
    , static_cast<Float>(0.01684000f), static_cast<Float>(0.01800736f), static_cast<Float>(0.01921448f)
    , static_cast<Float>(0.02045392f), static_cast<Float>(0.02171824f), static_cast<Float>(0.02300000f)
    , static_cast<Float>(0.02429461f), static_cast<Float>(0.02561024f), static_cast<Float>(0.02695857f)
    , static_cast<Float>(0.02835125f), static_cast<Float>(0.02980000f), static_cast<Float>(0.03131083f)
    , static_cast<Float>(0.03288368f), static_cast<Float>(0.03452112f), static_cast<Float>(0.03622571f)
    , static_cast<Float>(0.03800000f), static_cast<Float>(0.03984667f), static_cast<Float>(0.04176800f)
    , static_cast<Float>(0.04376600f), static_cast<Float>(0.04584267f), static_cast<Float>(0.04800000f)
    , static_cast<Float>(0.05024368f), static_cast<Float>(0.05257304f), static_cast<Float>(0.05498056f)
    , static_cast<Float>(0.05745872f), static_cast<Float>(0.06000000f), static_cast<Float>(0.06260197f)
    , static_cast<Float>(0.06527752f), static_cast<Float>(0.06804208f), static_cast<Float>(0.07091109f)
    , static_cast<Float>(0.07390000f), static_cast<Float>(0.07701600f), static_cast<Float>(0.08026640f)
    , static_cast<Float>(0.08366680f), static_cast<Float>(0.08723280f), static_cast<Float>(0.09098000f)
    , static_cast<Float>(0.09491755f), static_cast<Float>(0.09904584f), static_cast<Float>(0.1033674f)
    , static_cast<Float>(0.1078846f), static_cast<Float>(0.1126000f), static_cast<Float>(0.1175320f)
    , static_cast<Float>(0.1226744f), static_cast<Float>(0.1279928f), static_cast<Float>(0.1334528f)
    , static_cast<Float>(0.1390200f), static_cast<Float>(0.1446764f), static_cast<Float>(0.1504693f)
    , static_cast<Float>(0.1564619f), static_cast<Float>(0.1627177f), static_cast<Float>(0.1693000f)
    , static_cast<Float>(0.1762431f), static_cast<Float>(0.1835581f), static_cast<Float>(0.1912735f)
    , static_cast<Float>(0.1994180f), static_cast<Float>(0.2080200f), static_cast<Float>(0.2171199f)
    , static_cast<Float>(0.2267345f), static_cast<Float>(0.2368571f), static_cast<Float>(0.2474812f)
    , static_cast<Float>(0.2586000f), static_cast<Float>(0.2701849f), static_cast<Float>(0.2822939f)
    , static_cast<Float>(0.2950505f), static_cast<Float>(0.3085780f), static_cast<Float>(0.3230000f)
    , static_cast<Float>(0.3384021f), static_cast<Float>(0.3546858f), static_cast<Float>(0.3716986f)
    , static_cast<Float>(0.3892875f), static_cast<Float>(0.4073000f), static_cast<Float>(0.4256299f)
    , static_cast<Float>(0.4443096f), static_cast<Float>(0.4633944f), static_cast<Float>(0.4829395f)
    , static_cast<Float>(0.5030000f), static_cast<Float>(0.5235693f), static_cast<Float>(0.5445120f)
    , static_cast<Float>(0.5656900f), static_cast<Float>(0.5869653f), static_cast<Float>(0.6082000f)
    , static_cast<Float>(0.6293456f), static_cast<Float>(0.6503068f), static_cast<Float>(0.6708752f)
    , static_cast<Float>(0.6908424f), static_cast<Float>(0.7100000f), static_cast<Float>(0.7281852f)
    , static_cast<Float>(0.7454636f), static_cast<Float>(0.7619694f), static_cast<Float>(0.7778368f)
    , static_cast<Float>(0.7932000f), static_cast<Float>(0.8081104f), static_cast<Float>(0.8224962f)
    , static_cast<Float>(0.8363068f), static_cast<Float>(0.8494916f), static_cast<Float>(0.8620000f)
    , static_cast<Float>(0.8738108f), static_cast<Float>(0.8849624f), static_cast<Float>(0.8954936f)
    , static_cast<Float>(0.9054432f), static_cast<Float>(0.9148501f), static_cast<Float>(0.9237348f)
    , static_cast<Float>(0.9320924f), static_cast<Float>(0.9399226f), static_cast<Float>(0.9472252f)
    , static_cast<Float>(0.9540000f), static_cast<Float>(0.9602561f), static_cast<Float>(0.9660074f)
    , static_cast<Float>(0.9712606f), static_cast<Float>(0.9760225f), static_cast<Float>(0.9803000f)
    , static_cast<Float>(0.9840924f), static_cast<Float>(0.9874812f), static_cast<Float>(0.9903128f)
    , static_cast<Float>(0.9928116f), static_cast<Float>(0.9949501f), static_cast<Float>(0.9967108f)
    , static_cast<Float>(0.9980983f), static_cast<Float>(0.9991120f), static_cast<Float>(0.9997482f)
    , static_cast<Float>(1.0000000f), static_cast<Float>(0.9998567f), static_cast<Float>(0.9993046f)
    , static_cast<Float>(0.9983255f), static_cast<Float>(0.9968987f), static_cast<Float>(0.9950000f)
    , static_cast<Float>(0.9926005f), static_cast<Float>(0.9897426f), static_cast<Float>(0.9864444f)
    , static_cast<Float>(0.9827241f), static_cast<Float>(0.9786000f), static_cast<Float>(0.9740837f)
    , static_cast<Float>(0.9691712f), static_cast<Float>(0.9638568f), static_cast<Float>(0.9581349f)
    , static_cast<Float>(0.9520000f), static_cast<Float>(0.9454504f), static_cast<Float>(0.9384992f)
    , static_cast<Float>(0.9311628f), static_cast<Float>(0.9234576f), static_cast<Float>(0.9154000f)
    , static_cast<Float>(0.9070064f), static_cast<Float>(0.8982772f), static_cast<Float>(0.8892048f)
    , static_cast<Float>(0.8797816f), static_cast<Float>(0.8700000f), static_cast<Float>(0.8598613f)
    , static_cast<Float>(0.8493920f), static_cast<Float>(0.8386220f), static_cast<Float>(0.8275813f)
    , static_cast<Float>(0.8163000f), static_cast<Float>(0.8047947f), static_cast<Float>(0.7930820f)
    , static_cast<Float>(0.7811920f), static_cast<Float>(0.7691547f), static_cast<Float>(0.7570000f)
    , static_cast<Float>(0.7447541f), static_cast<Float>(0.7324224f), static_cast<Float>(0.7200036f)
    , static_cast<Float>(0.7074965f), static_cast<Float>(0.6949000f), static_cast<Float>(0.6822192f)
    , static_cast<Float>(0.6694716f), static_cast<Float>(0.6566744f), static_cast<Float>(0.6438448f)
    , static_cast<Float>(0.6310000f), static_cast<Float>(0.6181555f), static_cast<Float>(0.6053144f)
    , static_cast<Float>(0.5924756f), static_cast<Float>(0.5796379f), static_cast<Float>(0.5668000f)
    , static_cast<Float>(0.5539611f), static_cast<Float>(0.5411372f), static_cast<Float>(0.5283528f)
    , static_cast<Float>(0.5156323f), static_cast<Float>(0.5030000f), static_cast<Float>(0.4904688f)
    , static_cast<Float>(0.4780304f), static_cast<Float>(0.4656776f), static_cast<Float>(0.4534032f)
    , static_cast<Float>(0.4412000f), static_cast<Float>(0.4290800f), static_cast<Float>(0.4170360f)
    , static_cast<Float>(0.4050320f), static_cast<Float>(0.3930320f), static_cast<Float>(0.3810000f)
    , static_cast<Float>(0.3689184f), static_cast<Float>(0.3568272f), static_cast<Float>(0.3447768f)
    , static_cast<Float>(0.3328176f), static_cast<Float>(0.3210000f), static_cast<Float>(0.3093381f)
    , static_cast<Float>(0.2978504f), static_cast<Float>(0.2865936f), static_cast<Float>(0.2756245f)
    , static_cast<Float>(0.2650000f), static_cast<Float>(0.2547632f), static_cast<Float>(0.2448896f)
    , static_cast<Float>(0.2353344f), static_cast<Float>(0.2260528f), static_cast<Float>(0.2170000f)
    , static_cast<Float>(0.2081616f), static_cast<Float>(0.1995488f), static_cast<Float>(0.1911552f)
    , static_cast<Float>(0.1829744f), static_cast<Float>(0.1750000f), static_cast<Float>(0.1672235f)
    , static_cast<Float>(0.1596464f), static_cast<Float>(0.1522776f), static_cast<Float>(0.1451259f)
    , static_cast<Float>(0.1382000f), static_cast<Float>(0.1315003f), static_cast<Float>(0.1250248f)
    , static_cast<Float>(0.1187792f), static_cast<Float>(0.1127691f), static_cast<Float>(0.1070000f)
    , static_cast<Float>(0.1014762f), static_cast<Float>(0.09618864f), static_cast<Float>(0.09112296f)
    , static_cast<Float>(0.08626485f), static_cast<Float>(0.08160000f), static_cast<Float>(0.07712064f)
    , static_cast<Float>(0.07282552f), static_cast<Float>(0.06871008f), static_cast<Float>(0.06476976f)
    , static_cast<Float>(0.06100000f), static_cast<Float>(0.05739621f), static_cast<Float>(0.05395504f)
    , static_cast<Float>(0.05067376f), static_cast<Float>(0.04754965f), static_cast<Float>(0.04458000f)
    , static_cast<Float>(0.04175872f), static_cast<Float>(0.03908496f), static_cast<Float>(0.03656384f)
    , static_cast<Float>(0.03420048f), static_cast<Float>(0.03200000f), static_cast<Float>(0.02996261f)
    , static_cast<Float>(0.02807664f), static_cast<Float>(0.02632936f), static_cast<Float>(0.02470805f)
    , static_cast<Float>(0.02320000f), static_cast<Float>(0.02180077f), static_cast<Float>(0.02050112f)
    , static_cast<Float>(0.01928108f), static_cast<Float>(0.01812069f), static_cast<Float>(0.01700000f)
    , static_cast<Float>(0.01590379f), static_cast<Float>(0.01483718f), static_cast<Float>(0.01381068f)
    , static_cast<Float>(0.01283478f), static_cast<Float>(0.01192000f), static_cast<Float>(0.01106831f)
    , static_cast<Float>(0.01027339f), static_cast<Float>(0.009533311f), static_cast<Float>(0.008846157f)
    , static_cast<Float>(0.008210000f), static_cast<Float>(0.007623781f), static_cast<Float>(0.007085424f)
    , static_cast<Float>(0.006591476f), static_cast<Float>(0.006138485f), static_cast<Float>(0.005723000f)
    , static_cast<Float>(0.005343059f), static_cast<Float>(0.004995796f), static_cast<Float>(0.004676404f)
    , static_cast<Float>(0.004380075f), static_cast<Float>(0.004102000f), static_cast<Float>(0.003838453f)
    , static_cast<Float>(0.003589099f), static_cast<Float>(0.003354219f), static_cast<Float>(0.003134093f)
    , static_cast<Float>(0.002929000f), static_cast<Float>(0.002738139f), static_cast<Float>(0.002559876f)
    , static_cast<Float>(0.002393244f), static_cast<Float>(0.002237275f), static_cast<Float>(0.002091000f)
    , static_cast<Float>(0.001953587f), static_cast<Float>(0.001824580f), static_cast<Float>(0.001703580f)
    , static_cast<Float>(0.001590187f), static_cast<Float>(0.001484000f), static_cast<Float>(0.001384496f)
    , static_cast<Float>(0.001291268f), static_cast<Float>(0.001204092f), static_cast<Float>(0.001122744f)
    , static_cast<Float>(0.001047000f), static_cast<Float>(0.0009765896f), static_cast<Float>(0.0009111088f)
    , static_cast<Float>(0.0008501332f), static_cast<Float>(0.0007932384f), static_cast<Float>(0.0007400000f)
    , static_cast<Float>(0.0006900827f), static_cast<Float>(0.0006433100f), static_cast<Float>(0.0005994960f)
    , static_cast<Float>(0.0005584547f), static_cast<Float>(0.0005200000f), static_cast<Float>(0.0004839136f)
    , static_cast<Float>(0.0004500528f), static_cast<Float>(0.0004183452f), static_cast<Float>(0.0003887184f)
    , static_cast<Float>(0.0003611000f), static_cast<Float>(0.0003353835f), static_cast<Float>(0.0003114404f)
    , static_cast<Float>(0.0002891656f), static_cast<Float>(0.0002684539f), static_cast<Float>(0.0002492000f)
    , static_cast<Float>(0.0002313019f), static_cast<Float>(0.0002146856f), static_cast<Float>(0.0001992884f)
    , static_cast<Float>(0.0001850475f), static_cast<Float>(0.0001719000f), static_cast<Float>(0.0001597781f)
    , static_cast<Float>(0.0001486044f), static_cast<Float>(0.0001383016f), static_cast<Float>(0.0001287925f)
    , static_cast<Float>(0.0001200000f), static_cast<Float>(0.0001118595f), static_cast<Float>(0.0001043224f)
    , static_cast<Float>(0.00009733560f), static_cast<Float>(0.00009084587f), static_cast<Float>(0.00008480000f)
    , static_cast<Float>(0.00007914667f), static_cast<Float>(0.00007385800f), static_cast<Float>(0.00006891600f)
    , static_cast<Float>(0.00006430267f), static_cast<Float>(0.00006000000f), static_cast<Float>(0.00005598187f)
    , static_cast<Float>(0.00005222560f), static_cast<Float>(0.00004871840f), static_cast<Float>(0.00004544747f)
    , static_cast<Float>(0.00004240000f), static_cast<Float>(0.00003956104f), static_cast<Float>(0.00003691512f)
    , static_cast<Float>(0.00003444868f), static_cast<Float>(0.00003214816f), static_cast<Float>(0.00003000000f)
    , static_cast<Float>(0.00002799125f), static_cast<Float>(0.00002611356f), static_cast<Float>(0.00002436024f)
    , static_cast<Float>(0.00002272461f), static_cast<Float>(0.00002120000f), static_cast<Float>(0.00001977855f)
    , static_cast<Float>(0.00001845285f), static_cast<Float>(0.00001721687f), static_cast<Float>(0.00001606459f)
    , static_cast<Float>(0.00001499000f), static_cast<Float>(0.00001398728f), static_cast<Float>(0.00001305155f)
    , static_cast<Float>(0.00001217818f), static_cast<Float>(0.00001136254f), static_cast<Float>(0.00001060000f)
    , static_cast<Float>(0.000009885877f), static_cast<Float>(0.000009217304f), static_cast<Float>(0.000008592362f)
    , static_cast<Float>(0.000008009133f), static_cast<Float>(0.000007465700f), static_cast<Float>(0.000006959567f)
    , static_cast<Float>(0.000006487995f), static_cast<Float>(0.000006048699f), static_cast<Float>(0.000005639396f)
    , static_cast<Float>(0.000005257800f), static_cast<Float>(0.000004901771f), static_cast<Float>(0.000004569720f)
    , static_cast<Float>(0.000004260194f), static_cast<Float>(0.000003971739f), static_cast<Float>(0.000003702900f)
    , static_cast<Float>(0.000003452163f), static_cast<Float>(0.000003218302f), static_cast<Float>(0.000003000300f)
    , static_cast<Float>(0.000002797139f), static_cast<Float>(0.000002607800f), static_cast<Float>(0.000002431220f)
    , static_cast<Float>(0.000002266531f), static_cast<Float>(0.000002113013f), static_cast<Float>(0.000001969943f)
    , static_cast<Float>(0.000001836600f), static_cast<Float>(0.000001712230f), static_cast<Float>(0.000001596228f)
    , static_cast<Float>(0.000001488090f), static_cast<Float>(0.000001387314f), static_cast<Float>(0.000001293400f)
    , static_cast<Float>(0.000001205820f), static_cast<Float>(0.000001124143f), static_cast<Float>(0.000001048009f)
    , static_cast<Float>(0.0000009770578f), static_cast<Float>(0.0000009109300f), static_cast<Float>(0.0000008492513f)
    , static_cast<Float>(0.0000007917212f), static_cast<Float>(0.0000007380904f), static_cast<Float>(0.0000006881098f)
    , static_cast<Float>(0.0000006415300f), static_cast<Float>(0.0000005980895f), static_cast<Float>(0.0000005575746f)
    , static_cast<Float>(0.0000005198080f), static_cast<Float>(0.0000004846123f), static_cast<Float>(0.0000004518100f)};

// CIE Z function values
const Float CIE_Z[CIE_SAMPLES_NUMBER] = {
    static_cast<Float>(0.0006061000f), static_cast<Float>(0.0006808792f), static_cast<Float>(0.0007651456f)
    , static_cast<Float>(0.0008600124f), static_cast<Float>(0.0009665928f), static_cast<Float>(0.001086000f)
    , static_cast<Float>(0.001220586f), static_cast<Float>(0.001372729f), static_cast<Float>(0.001543579f)
    , static_cast<Float>(0.001734286f), static_cast<Float>(0.001946000f), static_cast<Float>(0.002177777f)
    , static_cast<Float>(0.002435809f), static_cast<Float>(0.002731953f), static_cast<Float>(0.003078064f)
    , static_cast<Float>(0.003486000f), static_cast<Float>(0.003975227f), static_cast<Float>(0.004540880f)
    , static_cast<Float>(0.005158320f), static_cast<Float>(0.005802907f), static_cast<Float>(0.006450001f)
    , static_cast<Float>(0.007083216f), static_cast<Float>(0.007745488f), static_cast<Float>(0.008501152f)
    , static_cast<Float>(0.009414544f), static_cast<Float>(0.01054999f), static_cast<Float>(0.01196580f)
    , static_cast<Float>(0.01365587f), static_cast<Float>(0.01558805f), static_cast<Float>(0.01773015f)
    , static_cast<Float>(0.02005001f), static_cast<Float>(0.02251136f), static_cast<Float>(0.02520288f)
    , static_cast<Float>(0.02827972f), static_cast<Float>(0.03189704f), static_cast<Float>(0.03621000f)
    , static_cast<Float>(0.04143771f), static_cast<Float>(0.04750372f), static_cast<Float>(0.05411988f)
    , static_cast<Float>(0.06099803f), static_cast<Float>(0.06785001f), static_cast<Float>(0.07448632f)
    , static_cast<Float>(0.08136156f), static_cast<Float>(0.08915364f), static_cast<Float>(0.09854048f)
    , static_cast<Float>(0.1102000f), static_cast<Float>(0.1246133f), static_cast<Float>(0.1417017f)
    , static_cast<Float>(0.1613035f), static_cast<Float>(0.1832568f), static_cast<Float>(0.2074000f)
    , static_cast<Float>(0.2336921f), static_cast<Float>(0.2626114f), static_cast<Float>(0.2947746f)
    , static_cast<Float>(0.3307985f), static_cast<Float>(0.3713000f), static_cast<Float>(0.4162091f)
    , static_cast<Float>(0.4654642f), static_cast<Float>(0.5196948f), static_cast<Float>(0.5795303f)
    , static_cast<Float>(0.6456000f), static_cast<Float>(0.7184838f), static_cast<Float>(0.7967133f)
    , static_cast<Float>(0.8778459f), static_cast<Float>(0.9594390f), static_cast<Float>(1.0390501f)
    , static_cast<Float>(1.1153673f), static_cast<Float>(1.1884971f), static_cast<Float>(1.2581233f)
    , static_cast<Float>(1.3239296f), static_cast<Float>(1.3856000f), static_cast<Float>(1.4426352f)
    , static_cast<Float>(1.4948035f), static_cast<Float>(1.5421903f), static_cast<Float>(1.5848807f)
    , static_cast<Float>(1.6229600f), static_cast<Float>(1.6564048f), static_cast<Float>(1.6852959f)
    , static_cast<Float>(1.7098745f), static_cast<Float>(1.7303821f), static_cast<Float>(1.7470600f)
    , static_cast<Float>(1.7600446f), static_cast<Float>(1.7696233f), static_cast<Float>(1.7762637f)
    , static_cast<Float>(1.7804334f), static_cast<Float>(1.7826000f), static_cast<Float>(1.7829682f)
    , static_cast<Float>(1.7816998f), static_cast<Float>(1.7791982f), static_cast<Float>(1.7758671f)
    , static_cast<Float>(1.7721100f), static_cast<Float>(1.7682589f), static_cast<Float>(1.7640390f)
    , static_cast<Float>(1.7589438f), static_cast<Float>(1.7524663f), static_cast<Float>(1.7441000f)
    , static_cast<Float>(1.7335595f), static_cast<Float>(1.7208581f), static_cast<Float>(1.7059369f)
    , static_cast<Float>(1.6887372f), static_cast<Float>(1.6692000f), static_cast<Float>(1.6475287f)
    , static_cast<Float>(1.6234127f), static_cast<Float>(1.5960223f), static_cast<Float>(1.5645280f)
    , static_cast<Float>(1.5281000f), static_cast<Float>(1.4861114f), static_cast<Float>(1.4395215f)
    , static_cast<Float>(1.3898799f), static_cast<Float>(1.3387362f), static_cast<Float>(1.2876400f)
    , static_cast<Float>(1.2374223f), static_cast<Float>(1.1878243f), static_cast<Float>(1.1387611f)
    , static_cast<Float>(1.0901480f), static_cast<Float>(1.0419000f), static_cast<Float>(0.9941976f)
    , static_cast<Float>(0.9473473f), static_cast<Float>(0.9014531f), static_cast<Float>(0.8566193f)
    , static_cast<Float>(0.8129501f), static_cast<Float>(0.7705173f), static_cast<Float>(0.7294448f)
    , static_cast<Float>(0.6899136f), static_cast<Float>(0.6521049f), static_cast<Float>(0.6162000f)
    , static_cast<Float>(0.5823286f), static_cast<Float>(0.5504162f), static_cast<Float>(0.5203376f)
    , static_cast<Float>(0.4919673f), static_cast<Float>(0.4651800f), static_cast<Float>(0.4399246f)
    , static_cast<Float>(0.4161836f), static_cast<Float>(0.3938822f), static_cast<Float>(0.3729459f)
    , static_cast<Float>(0.3533000f), static_cast<Float>(0.3348578f), static_cast<Float>(0.3175521f)
    , static_cast<Float>(0.3013375f), static_cast<Float>(0.2861686f), static_cast<Float>(0.2720000f)
    , static_cast<Float>(0.2588171f), static_cast<Float>(0.2464838f), static_cast<Float>(0.2347718f)
    , static_cast<Float>(0.2234533f), static_cast<Float>(0.2123000f), static_cast<Float>(0.2011692f)
    , static_cast<Float>(0.1901196f), static_cast<Float>(0.1792254f), static_cast<Float>(0.1685608f)
    , static_cast<Float>(0.1582000f), static_cast<Float>(0.1481383f), static_cast<Float>(0.1383758f)
    , static_cast<Float>(0.1289942f), static_cast<Float>(0.1200751f), static_cast<Float>(0.1117000f)
    , static_cast<Float>(0.1039048f), static_cast<Float>(0.09666748f), static_cast<Float>(0.08998272f)
    , static_cast<Float>(0.08384531f), static_cast<Float>(0.07824999f), static_cast<Float>(0.07320899f)
    , static_cast<Float>(0.06867816f), static_cast<Float>(0.06456784f), static_cast<Float>(0.06078835f)
    , static_cast<Float>(0.05725001f), static_cast<Float>(0.05390435f), static_cast<Float>(0.05074664f)
    , static_cast<Float>(0.04775276f), static_cast<Float>(0.04489859f), static_cast<Float>(0.04216000f)
    , static_cast<Float>(0.03950728f), static_cast<Float>(0.03693564f), static_cast<Float>(0.03445836f)
    , static_cast<Float>(0.03208872f), static_cast<Float>(0.02984000f), static_cast<Float>(0.02771181f)
    , static_cast<Float>(0.02569444f), static_cast<Float>(0.02378716f), static_cast<Float>(0.02198925f)
    , static_cast<Float>(0.02030000f), static_cast<Float>(0.01871805f), static_cast<Float>(0.01724036f)
    , static_cast<Float>(0.01586364f), static_cast<Float>(0.01458461f), static_cast<Float>(0.01340000f)
    , static_cast<Float>(0.01230723f), static_cast<Float>(0.01130188f), static_cast<Float>(0.01037792f)
    , static_cast<Float>(0.009529306f), static_cast<Float>(0.008749999f), static_cast<Float>(0.008035200f)
    , static_cast<Float>(0.007381600f), static_cast<Float>(0.006785400f), static_cast<Float>(0.006242800f)
    , static_cast<Float>(0.005749999f), static_cast<Float>(0.005303600f), static_cast<Float>(0.004899800f)
    , static_cast<Float>(0.004534200f), static_cast<Float>(0.004202400f), static_cast<Float>(0.003900000f)
    , static_cast<Float>(0.003623200f), static_cast<Float>(0.003370600f), static_cast<Float>(0.003141400f)
    , static_cast<Float>(0.002934800f), static_cast<Float>(0.002749999f), static_cast<Float>(0.002585200f)
    , static_cast<Float>(0.002438600f), static_cast<Float>(0.002309400f), static_cast<Float>(0.002196800f)
    , static_cast<Float>(0.002100000f), static_cast<Float>(0.002017733f), static_cast<Float>(0.001948200f)
    , static_cast<Float>(0.001889800f), static_cast<Float>(0.001840933f), static_cast<Float>(0.001800000f)
    , static_cast<Float>(0.001766267f), static_cast<Float>(0.001737800f), static_cast<Float>(0.001711200f)
    , static_cast<Float>(0.001683067f), static_cast<Float>(0.001650001f), static_cast<Float>(0.001610133f)
    , static_cast<Float>(0.001564400f), static_cast<Float>(0.001513600f), static_cast<Float>(0.001458533f)
    , static_cast<Float>(0.001400000f), static_cast<Float>(0.001336667f), static_cast<Float>(0.001270000f)
    , static_cast<Float>(0.001205000f), static_cast<Float>(0.001146667f), static_cast<Float>(0.001100000f)
    , static_cast<Float>(0.001068800f), static_cast<Float>(0.001049400f), static_cast<Float>(0.001035600f)
    , static_cast<Float>(0.001021200f), static_cast<Float>(0.001000000f), static_cast<Float>(0.0009686400f)
    , static_cast<Float>(0.0009299200f), static_cast<Float>(0.0008868800f), static_cast<Float>(0.0008425600f)
    , static_cast<Float>(0.0008000000f), static_cast<Float>(0.0007609600f), static_cast<Float>(0.0007236800f)
    , static_cast<Float>(0.0006859200f), static_cast<Float>(0.0006454400f), static_cast<Float>(0.0006000000f)
    , static_cast<Float>(0.0005478667f), static_cast<Float>(0.0004916000f), static_cast<Float>(0.0004354000f)
    , static_cast<Float>(0.0003834667f), static_cast<Float>(0.0003400000f), static_cast<Float>(0.0003072533f)
    , static_cast<Float>(0.0002831600f), static_cast<Float>(0.0002654400f), static_cast<Float>(0.0002518133f)
    , static_cast<Float>(0.0002400000f), static_cast<Float>(0.0002295467f), static_cast<Float>(0.0002206400f)
    , static_cast<Float>(0.0002119600f), static_cast<Float>(0.0002021867f), static_cast<Float>(0.0001900000f)
    , static_cast<Float>(0.0001742133f), static_cast<Float>(0.0001556400f), static_cast<Float>(0.0001359600f)
    , static_cast<Float>(0.0001168533f), static_cast<Float>(0.0001000000f), static_cast<Float>(0.00008613333f)
    , static_cast<Float>(0.00007460000f), static_cast<Float>(0.00006500000f), static_cast<Float>(0.00005693333f)
    , static_cast<Float>(0.00004999999f), static_cast<Float>(0.00004416000f), static_cast<Float>(0.00003948000f)
    , static_cast<Float>(0.00003572000f), static_cast<Float>(0.00003264000f), static_cast<Float>(0.00003000000f)
    , static_cast<Float>(0.00002765333f), static_cast<Float>(0.00002556000f), static_cast<Float>(0.00002364000f)
    , static_cast<Float>(0.00002181333f), static_cast<Float>(0.00002000000f), static_cast<Float>(0.00001813333f)
    , static_cast<Float>(0.00001620000f), static_cast<Float>(0.00001420000f), static_cast<Float>(0.00001213333f)
    , static_cast<Float>(0.00001000000f), static_cast<Float>(0.000007733333f), static_cast<Float>(0.000005400000f)
    , static_cast<Float>(0.000003200000f), static_cast<Float>(0.000001333333f), static_cast<Float>(0.000000000000f)
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0
    , FLOAT_0, FLOAT_0, FLOAT_0};

const Float CIE_lambda[CIE_SAMPLES_NUMBER] = {
    static_cast<Float>(360), static_cast<Float>(361), static_cast<Float>(362)
    , static_cast<Float>(363), static_cast<Float>(364), static_cast<Float>(365)
    , static_cast<Float>(366), static_cast<Float>(367), static_cast<Float>(368)
    , static_cast<Float>(369), static_cast<Float>(370), static_cast<Float>(371)
    , static_cast<Float>(372), static_cast<Float>(373), static_cast<Float>(374)
    , static_cast<Float>(375), static_cast<Float>(376), static_cast<Float>(377)
    , static_cast<Float>(378), static_cast<Float>(379), static_cast<Float>(380)
    , static_cast<Float>(381), static_cast<Float>(382), static_cast<Float>(383)
    , static_cast<Float>(384), static_cast<Float>(385), static_cast<Float>(386)
    , static_cast<Float>(387), static_cast<Float>(388), static_cast<Float>(389)
    , static_cast<Float>(390), static_cast<Float>(391), static_cast<Float>(392)
    , static_cast<Float>(393), static_cast<Float>(394), static_cast<Float>(395)
    , static_cast<Float>(396), static_cast<Float>(397), static_cast<Float>(398)
    , static_cast<Float>(399), static_cast<Float>(400), static_cast<Float>(401)
    , static_cast<Float>(402), static_cast<Float>(403), static_cast<Float>(404)
    , static_cast<Float>(405), static_cast<Float>(406), static_cast<Float>(407)
    , static_cast<Float>(408), static_cast<Float>(409), static_cast<Float>(410)
    , static_cast<Float>(411), static_cast<Float>(412), static_cast<Float>(413)
    , static_cast<Float>(414), static_cast<Float>(415), static_cast<Float>(416)
    , static_cast<Float>(417), static_cast<Float>(418), static_cast<Float>(419)
    , static_cast<Float>(420), static_cast<Float>(421), static_cast<Float>(422)
    , static_cast<Float>(423), static_cast<Float>(424), static_cast<Float>(425)
    , static_cast<Float>(426), static_cast<Float>(427), static_cast<Float>(428)
    , static_cast<Float>(429), static_cast<Float>(430), static_cast<Float>(431)
    , static_cast<Float>(432), static_cast<Float>(433), static_cast<Float>(434)
    , static_cast<Float>(435), static_cast<Float>(436), static_cast<Float>(437)
    , static_cast<Float>(438), static_cast<Float>(439), static_cast<Float>(440)
    , static_cast<Float>(441), static_cast<Float>(442), static_cast<Float>(443)
    , static_cast<Float>(444), static_cast<Float>(445), static_cast<Float>(446)
    , static_cast<Float>(447), static_cast<Float>(448), static_cast<Float>(449)
    , static_cast<Float>(450), static_cast<Float>(451), static_cast<Float>(452)
    , static_cast<Float>(453), static_cast<Float>(454), static_cast<Float>(455)
    , static_cast<Float>(456), static_cast<Float>(457), static_cast<Float>(458)
    , static_cast<Float>(459), static_cast<Float>(460), static_cast<Float>(461)
    , static_cast<Float>(462), static_cast<Float>(463), static_cast<Float>(464)
    , static_cast<Float>(465), static_cast<Float>(466), static_cast<Float>(467)
    , static_cast<Float>(468), static_cast<Float>(469), static_cast<Float>(470)
    , static_cast<Float>(471), static_cast<Float>(472), static_cast<Float>(473)
    , static_cast<Float>(474), static_cast<Float>(475), static_cast<Float>(476)
    , static_cast<Float>(477), static_cast<Float>(478), static_cast<Float>(479)
    , static_cast<Float>(480), static_cast<Float>(481), static_cast<Float>(482)
    , static_cast<Float>(483), static_cast<Float>(484), static_cast<Float>(485)
    , static_cast<Float>(486), static_cast<Float>(487), static_cast<Float>(488)
    , static_cast<Float>(489), static_cast<Float>(490), static_cast<Float>(491)
    , static_cast<Float>(492), static_cast<Float>(493), static_cast<Float>(494)
    , static_cast<Float>(495), static_cast<Float>(496), static_cast<Float>(497)
    , static_cast<Float>(498), static_cast<Float>(499), static_cast<Float>(500)
    , static_cast<Float>(501), static_cast<Float>(502), static_cast<Float>(503)
    , static_cast<Float>(504), static_cast<Float>(505), static_cast<Float>(506)
    , static_cast<Float>(507), static_cast<Float>(508), static_cast<Float>(509)
    , static_cast<Float>(510), static_cast<Float>(511), static_cast<Float>(512)
    , static_cast<Float>(513), static_cast<Float>(514), static_cast<Float>(515)
    , static_cast<Float>(516), static_cast<Float>(517), static_cast<Float>(518)
    , static_cast<Float>(519), static_cast<Float>(520), static_cast<Float>(521)
    , static_cast<Float>(522), static_cast<Float>(523), static_cast<Float>(524)
    , static_cast<Float>(525), static_cast<Float>(526), static_cast<Float>(527)
    , static_cast<Float>(528), static_cast<Float>(529), static_cast<Float>(530)
    , static_cast<Float>(531), static_cast<Float>(532), static_cast<Float>(533)
    , static_cast<Float>(534), static_cast<Float>(535), static_cast<Float>(536)
    , static_cast<Float>(537), static_cast<Float>(538), static_cast<Float>(539)
    , static_cast<Float>(540), static_cast<Float>(541), static_cast<Float>(542)
    , static_cast<Float>(543), static_cast<Float>(544), static_cast<Float>(545)
    , static_cast<Float>(546), static_cast<Float>(547), static_cast<Float>(548)
    , static_cast<Float>(549), static_cast<Float>(550), static_cast<Float>(551)
    , static_cast<Float>(552), static_cast<Float>(553), static_cast<Float>(554)
    , static_cast<Float>(555), static_cast<Float>(556), static_cast<Float>(557)
    , static_cast<Float>(558), static_cast<Float>(559), static_cast<Float>(560)
    , static_cast<Float>(561), static_cast<Float>(562), static_cast<Float>(563)
    , static_cast<Float>(564), static_cast<Float>(565), static_cast<Float>(566)
    , static_cast<Float>(567), static_cast<Float>(568), static_cast<Float>(569)
    , static_cast<Float>(570), static_cast<Float>(571), static_cast<Float>(572)
    , static_cast<Float>(573), static_cast<Float>(574), static_cast<Float>(575)
    , static_cast<Float>(576), static_cast<Float>(577), static_cast<Float>(578)
    , static_cast<Float>(579), static_cast<Float>(580), static_cast<Float>(581)
    , static_cast<Float>(582), static_cast<Float>(583), static_cast<Float>(584)
    , static_cast<Float>(585), static_cast<Float>(586), static_cast<Float>(587)
    , static_cast<Float>(588), static_cast<Float>(589), static_cast<Float>(590)
    , static_cast<Float>(591), static_cast<Float>(592), static_cast<Float>(593)
    , static_cast<Float>(594), static_cast<Float>(595), static_cast<Float>(596)
    , static_cast<Float>(597), static_cast<Float>(598), static_cast<Float>(599)
    , static_cast<Float>(600), static_cast<Float>(601), static_cast<Float>(602)
    , static_cast<Float>(603), static_cast<Float>(604), static_cast<Float>(605)
    , static_cast<Float>(606), static_cast<Float>(607), static_cast<Float>(608)
    , static_cast<Float>(609), static_cast<Float>(610), static_cast<Float>(611)
    , static_cast<Float>(612), static_cast<Float>(613), static_cast<Float>(614)
    , static_cast<Float>(615), static_cast<Float>(616), static_cast<Float>(617)
    , static_cast<Float>(618), static_cast<Float>(619), static_cast<Float>(620)
    , static_cast<Float>(621), static_cast<Float>(622), static_cast<Float>(623)
    , static_cast<Float>(624), static_cast<Float>(625), static_cast<Float>(626)
    , static_cast<Float>(627), static_cast<Float>(628), static_cast<Float>(629)
    , static_cast<Float>(630), static_cast<Float>(631), static_cast<Float>(632)
    , static_cast<Float>(633), static_cast<Float>(634), static_cast<Float>(635)
    , static_cast<Float>(636), static_cast<Float>(637), static_cast<Float>(638)
    , static_cast<Float>(639), static_cast<Float>(640), static_cast<Float>(641)
    , static_cast<Float>(642), static_cast<Float>(643), static_cast<Float>(644)
    , static_cast<Float>(645), static_cast<Float>(646), static_cast<Float>(647)
    , static_cast<Float>(648), static_cast<Float>(649), static_cast<Float>(650)
    , static_cast<Float>(651), static_cast<Float>(652), static_cast<Float>(653)
    , static_cast<Float>(654), static_cast<Float>(655), static_cast<Float>(656)
    , static_cast<Float>(657), static_cast<Float>(658), static_cast<Float>(659)
    , static_cast<Float>(660), static_cast<Float>(661), static_cast<Float>(662)
    , static_cast<Float>(663), static_cast<Float>(664), static_cast<Float>(665)
    , static_cast<Float>(666), static_cast<Float>(667), static_cast<Float>(668)
    , static_cast<Float>(669), static_cast<Float>(670), static_cast<Float>(671)
    , static_cast<Float>(672), static_cast<Float>(673), static_cast<Float>(674)
    , static_cast<Float>(675), static_cast<Float>(676), static_cast<Float>(677)
    , static_cast<Float>(678), static_cast<Float>(679), static_cast<Float>(680)
    , static_cast<Float>(681), static_cast<Float>(682), static_cast<Float>(683)
    , static_cast<Float>(684), static_cast<Float>(685), static_cast<Float>(686)
    , static_cast<Float>(687), static_cast<Float>(688), static_cast<Float>(689)
    , static_cast<Float>(690), static_cast<Float>(691), static_cast<Float>(692)
    , static_cast<Float>(693), static_cast<Float>(694), static_cast<Float>(695)
    , static_cast<Float>(696), static_cast<Float>(697), static_cast<Float>(698)
    , static_cast<Float>(699), static_cast<Float>(700), static_cast<Float>(701)
    , static_cast<Float>(702), static_cast<Float>(703), static_cast<Float>(704)
    , static_cast<Float>(705), static_cast<Float>(706), static_cast<Float>(707)
    , static_cast<Float>(708), static_cast<Float>(709), static_cast<Float>(710)
    , static_cast<Float>(711), static_cast<Float>(712), static_cast<Float>(713)
    , static_cast<Float>(714), static_cast<Float>(715), static_cast<Float>(716)
    , static_cast<Float>(717), static_cast<Float>(718), static_cast<Float>(719)
    , static_cast<Float>(720), static_cast<Float>(721), static_cast<Float>(722)
    , static_cast<Float>(723), static_cast<Float>(724), static_cast<Float>(725)
    , static_cast<Float>(726), static_cast<Float>(727), static_cast<Float>(728)
    , static_cast<Float>(729), static_cast<Float>(730), static_cast<Float>(731)
    , static_cast<Float>(732), static_cast<Float>(733), static_cast<Float>(734)
    , static_cast<Float>(735), static_cast<Float>(736), static_cast<Float>(737)
    , static_cast<Float>(738), static_cast<Float>(739), static_cast<Float>(740)
    , static_cast<Float>(741), static_cast<Float>(742), static_cast<Float>(743)
    , static_cast<Float>(744), static_cast<Float>(745), static_cast<Float>(746)
    , static_cast<Float>(747), static_cast<Float>(748), static_cast<Float>(749)
    , static_cast<Float>(750), static_cast<Float>(751), static_cast<Float>(752)
    , static_cast<Float>(753), static_cast<Float>(754), static_cast<Float>(755)
    , static_cast<Float>(756), static_cast<Float>(757), static_cast<Float>(758)
    , static_cast<Float>(759), static_cast<Float>(760), static_cast<Float>(761)
    , static_cast<Float>(762), static_cast<Float>(763), static_cast<Float>(764)
    , static_cast<Float>(765), static_cast<Float>(766), static_cast<Float>(767)
    , static_cast<Float>(768), static_cast<Float>(769), static_cast<Float>(770)
    , static_cast<Float>(771), static_cast<Float>(772), static_cast<Float>(773)
    , static_cast<Float>(774), static_cast<Float>(775), static_cast<Float>(776)
    , static_cast<Float>(777), static_cast<Float>(778), static_cast<Float>(779)
    , static_cast<Float>(780), static_cast<Float>(781), static_cast<Float>(782)
    , static_cast<Float>(783), static_cast<Float>(784), static_cast<Float>(785)
    , static_cast<Float>(786), static_cast<Float>(787), static_cast<Float>(788)
    , static_cast<Float>(789), static_cast<Float>(790), static_cast<Float>(791)
    , static_cast<Float>(792), static_cast<Float>(793), static_cast<Float>(794)
    , static_cast<Float>(795), static_cast<Float>(796), static_cast<Float>(797)
    , static_cast<Float>(798), static_cast<Float>(799), static_cast<Float>(800)
    , static_cast<Float>(801), static_cast<Float>(802), static_cast<Float>(803)
    , static_cast<Float>(804), static_cast<Float>(805), static_cast<Float>(806)
    , static_cast<Float>(807), static_cast<Float>(808), static_cast<Float>(809)
    , static_cast<Float>(810), static_cast<Float>(811), static_cast<Float>(812)
    , static_cast<Float>(813), static_cast<Float>(814), static_cast<Float>(815)
    , static_cast<Float>(816), static_cast<Float>(817), static_cast<Float>(818)
    , static_cast<Float>(819), static_cast<Float>(820), static_cast<Float>(821)
    , static_cast<Float>(822), static_cast<Float>(823), static_cast<Float>(824)
    , static_cast<Float>(825), static_cast<Float>(826), static_cast<Float>(827)
    , static_cast<Float>(828), static_cast<Float>(829), static_cast<Float>(830)};

const Float RGB_2_SPECT_LAMBDA[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(380.000000), static_cast<Float>(390.967743), static_cast<Float>(401.935486)
    , static_cast<Float>(412.903229), static_cast<Float>(423.870972), static_cast<Float>(434.838715)
    , static_cast<Float>(445.806458), static_cast<Float>(456.774200), static_cast<Float>(467.741943)
    , static_cast<Float>(478.709686), static_cast<Float>(489.677429), static_cast<Float>(500.645172)
    , static_cast<Float>(511.612915), static_cast<Float>(522.580627), static_cast<Float>(533.548340)
    , static_cast<Float>(544.516052), static_cast<Float>(555.483765), static_cast<Float>(566.451477)
    , static_cast<Float>(577.419189), static_cast<Float>(588.386902), static_cast<Float>(599.354614)
    , static_cast<Float>(610.322327), static_cast<Float>(621.290039), static_cast<Float>(632.257751)
    , static_cast<Float>(643.225464), static_cast<Float>(654.193176), static_cast<Float>(665.160889)
    , static_cast<Float>(676.128601), static_cast<Float>(687.096313), static_cast<Float>(698.064026)
    , static_cast<Float>(709.031738), static_cast<Float>(720.000000)};

const Float RGB_REFL_2_SPECT_WHITE[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.0618958571272863e+00), static_cast<Float>(1.0615019980348779e+00)
    , static_cast<Float>(1.0614335379927147e+00), static_cast<Float>(1.0622711654692485e+00)
    , static_cast<Float>(1.0622036218416742e+00), static_cast<Float>(1.0625059965187085e+00)
    , static_cast<Float>(1.0623938486985884e+00), static_cast<Float>(1.0624706448043137e+00)
    , static_cast<Float>(1.0625048144827762e+00), static_cast<Float>(1.0624366131308856e+00)
    , static_cast<Float>(1.0620694238892607e+00), static_cast<Float>(1.0613167586932164e+00)
    , static_cast<Float>(1.0610334029377020e+00), static_cast<Float>(1.0613868564828413e+00)
    , static_cast<Float>(1.0614215366116762e+00), static_cast<Float>(1.0620336151299086e+00)
    , static_cast<Float>(1.0625497454805051e+00), static_cast<Float>(1.0624317487992085e+00)
    , static_cast<Float>(1.0625249140554480e+00), static_cast<Float>(1.0624277664486914e+00)
    , static_cast<Float>(1.0624749854090769e+00), static_cast<Float>(1.0625538581025402e+00)
    , static_cast<Float>(1.0625326910104864e+00), static_cast<Float>(1.0623922312225325e+00)
    , static_cast<Float>(1.0623650980354129e+00), static_cast<Float>(1.0625256476715284e+00)
    , static_cast<Float>(1.0612277619533155e+00), static_cast<Float>(1.0594262608698046e+00)
    , static_cast<Float>(1.0599810758292072e+00), static_cast<Float>(1.0602547314449409e+00)
    , static_cast<Float>(1.0601263046243634e+00), static_cast<Float>(1.0606565756823634e+00)};

const Float RGB_REFL_2_SPECT_CYAN[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.0414628021426751e+00), static_cast<Float>(1.0328661533771188e+00)
    , static_cast<Float>(1.0126146228964314e+00), static_cast<Float>(1.0350460524836209e+00)
    , static_cast<Float>(1.0078661447098567e+00), static_cast<Float>(1.0422280385081280e+00)
    , static_cast<Float>(1.0442596738499825e+00), static_cast<Float>(1.0535238290294409e+00)
    , static_cast<Float>(1.0180776226938120e+00), static_cast<Float>(1.0442729908727713e+00)
    , static_cast<Float>(1.0529362541920750e+00), static_cast<Float>(1.0537034271160244e+00)
    , static_cast<Float>(1.0533901869215969e+00), static_cast<Float>(1.0537782700979574e+00)
    , static_cast<Float>(1.0527093770467102e+00), static_cast<Float>(1.0530449040446797e+00)
    , static_cast<Float>(1.0550554640191208e+00), static_cast<Float>(1.0553673610724821e+00)
    , static_cast<Float>(1.0454306634683976e+00), static_cast<Float>(6.2348950639230805e-01)
    , static_cast<Float>(1.8038071613188977e-01), static_cast<Float>(-7.6303759201984539e-03)
    , static_cast<Float>(-1.5217847035781367e-04), static_cast<Float>(-7.5102257347258311e-03)
    , static_cast<Float>(-2.1708639328491472e-03), static_cast<Float>(6.5919466602369636e-04)
    , static_cast<Float>(1.2278815318539780e-02), static_cast<Float>(-4.4669775637208031e-03)
    , static_cast<Float>(1.7119799082865147e-02), static_cast<Float>(4.9211089759759801e-03)
    , static_cast<Float>(5.8762925143334985e-03), static_cast<Float>(2.5259399415550079e-02)};

const Float RGB_REFL_2_SPECT_MAGENTA[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(9.9422138151236850e-01), static_cast<Float>(9.8986937122975682e-01)
    , static_cast<Float>(9.8293658286116958e-01), static_cast<Float>(9.9627868399859310e-01)
    , static_cast<Float>(1.0198955019000133e+00), static_cast<Float>(1.0166395501210359e+00)
    , static_cast<Float>(1.0220913178757398e+00), static_cast<Float>(9.9651666040682441e-01)
    , static_cast<Float>(1.0097766178917882e+00), static_cast<Float>(1.0215422470827016e+00)
    , static_cast<Float>(6.4031953387790963e-01), static_cast<Float>(2.5012379477078184e-03)
    , static_cast<Float>(6.5339939555769944e-03), static_cast<Float>(2.8334080462675826e-03)
    , static_cast<Float>(-5.1209675389074505e-11), static_cast<Float>(-9.0592291646646381e-03)
    , static_cast<Float>(3.3936718323331200e-03), static_cast<Float>(-3.0638741121828406e-03)
    , static_cast<Float>(2.2203936168286292e-01), static_cast<Float>(6.3141140024811970e-01)
    , static_cast<Float>(9.7480985576500956e-01), static_cast<Float>(9.7209562333590571e-01)
    , static_cast<Float>(1.0173770302868150e+00), static_cast<Float>(9.9875194322734129e-01)
    , static_cast<Float>(9.4701725739602238e-01), static_cast<Float>(8.5258623154354796e-01)
    , static_cast<Float>(9.4897798581660842e-01), static_cast<Float>(9.4751876096521492e-01)
    , static_cast<Float>(9.9598944191059791e-01), static_cast<Float>(8.6301351503809076e-01)
    , static_cast<Float>(8.9150987853523145e-01), static_cast<Float>(8.4866492652845082e-01)};

const Float RGB_REFL_2_SPECT_YELLOW[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(5.5740622924920873e-03), static_cast<Float>(-4.7982831631446787e-03)
    , static_cast<Float>(-5.2536564298613798e-03), static_cast<Float>(-6.4571480044499710e-03)
    , static_cast<Float>(-5.9693514658007013e-03), static_cast<Float>(-2.1836716037686721e-03)
    , static_cast<Float>(1.6781120601055327e-02), static_cast<Float>(9.6096355429062641e-02)
    , static_cast<Float>(2.1217357081986446e-01), static_cast<Float>(3.6169133290685068e-01)
    , static_cast<Float>(5.3961011543232529e-01), static_cast<Float>(7.4408810492171507e-01)
    , static_cast<Float>(9.2209571148394054e-01), static_cast<Float>(1.0460304298411225e+00)
    , static_cast<Float>(1.0513824989063714e+00), static_cast<Float>(1.0511991822135085e+00)
    , static_cast<Float>(1.0510530911991052e+00), static_cast<Float>(1.0517397230360510e+00)
    , static_cast<Float>(1.0516043086790485e+00), static_cast<Float>(1.0511944032061460e+00)
    , static_cast<Float>(1.0511590325868068e+00), static_cast<Float>(1.0516612465483031e+00)
    , static_cast<Float>(1.0514038526836869e+00), static_cast<Float>(1.0515941029228475e+00)
    , static_cast<Float>(1.0511460436960840e+00), static_cast<Float>(1.0515123758830476e+00)
    , static_cast<Float>(1.0508871369510702e+00), static_cast<Float>(1.0508923708102380e+00)
    , static_cast<Float>(1.0477492815668303e+00), static_cast<Float>(1.0493272144017338e+00)
    , static_cast<Float>(1.0435963333422726e+00), static_cast<Float>(1.0392280772051465e+00)};

const Float RGB_REFL_2_SPECT_RED[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.6575604867086180e-01), static_cast<Float>(1.1846442802747797e-01)
    , static_cast<Float>(1.2408293329637447e-01), static_cast<Float>(1.1371272058349924e-01)
    , static_cast<Float>(7.8992434518899132e-02), static_cast<Float>(3.2205603593106549e-02)
    , static_cast<Float>(-1.0798365407877875e-02), static_cast<Float>(1.8051975516730392e-02)
    , static_cast<Float>(5.3407196598730527e-03), static_cast<Float>(1.3654918729501336e-02)
    , static_cast<Float>(-5.9564213545642841e-03), static_cast<Float>(-1.8444365067353252e-03)
    , static_cast<Float>(-1.0571884361529504e-02), static_cast<Float>(-2.9375521078000011e-03)
    , static_cast<Float>(-1.0790476271835936e-02), static_cast<Float>(-8.0224306697503633e-03)
    , static_cast<Float>(-2.2669167702495940e-03), static_cast<Float>(7.0200240494706634e-03)
    , static_cast<Float>(-8.1528469000299308e-03), static_cast<Float>(6.0772866969252792e-01)
    , static_cast<Float>(9.8831560865432400e-01), static_cast<Float>(9.9391691044078823e-01)
    , static_cast<Float>(1.0039338994753197e+00), static_cast<Float>(9.9234499861167125e-01)
    , static_cast<Float>(9.9926530858855522e-01), static_cast<Float>(1.0084621557617270e+00)
    , static_cast<Float>(9.8358296827441216e-01), static_cast<Float>(1.0085023660099048e+00)
    , static_cast<Float>(9.7451138326568698e-01), static_cast<Float>(9.8543269570059944e-01)
    , static_cast<Float>(9.3495763980962043e-01), static_cast<Float>(9.8713907792319400e-01)};

const Float RGB_REFL_2_SPECT_GREEN[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(2.6494153587602255e-03), static_cast<Float>(-5.0175013429732242e-03)
    , static_cast<Float>(-1.2547236272489583e-02), static_cast<Float>(-9.4554964308388671e-03)
    , static_cast<Float>(-1.2526086181600525e-02), static_cast<Float>(-7.9170697760437767e-03)
    , static_cast<Float>(-7.9955735204175690e-03), static_cast<Float>(-9.3559433444469070e-03)
    , static_cast<Float>(6.5468611982999303e-02), static_cast<Float>(3.9572875517634137e-01)
    , static_cast<Float>(7.5244022299886659e-01), static_cast<Float>(9.6376478690218559e-01)
    , static_cast<Float>(9.9854433855162328e-01), static_cast<Float>(9.9992977025287921e-01)
    , static_cast<Float>(9.9939086751140449e-01), static_cast<Float>(9.9994372267071396e-01)
    , static_cast<Float>(9.9939121813418674e-01), static_cast<Float>(9.9911237310424483e-01)
    , static_cast<Float>(9.6019584878271580e-01), static_cast<Float>(6.3186279338432438e-01)
    , static_cast<Float>(2.5797401028763473e-01), static_cast<Float>(9.4014888527335638e-03)
    , static_cast<Float>(-3.0798345608649747e-03), static_cast<Float>(-4.5230367033685034e-03)
    , static_cast<Float>(-6.8933410388274038e-03), static_cast<Float>(-9.0352195539015398e-03)
    , static_cast<Float>(-8.5913667165340209e-03), static_cast<Float>(-8.3690869120289398e-03)
    , static_cast<Float>(-7.8685832338754313e-03), static_cast<Float>(-8.3657578711085132e-06)
    , static_cast<Float>(5.4301225442817177e-03), static_cast<Float>(-2.7745589759259194e-03)};

const Float RGB_REFL_2_SPECT_BLUE[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(9.9209771469720676e-01), static_cast<Float>(9.8876426059369127e-01)
    , static_cast<Float>(9.9539040744505636e-01), static_cast<Float>(9.9529317353008218e-01)
    , static_cast<Float>(9.9181447411633950e-01), static_cast<Float>(1.0002584039673432e+00)
    , static_cast<Float>(9.9968478437342512e-01), static_cast<Float>(9.9988120766657174e-01)
    , static_cast<Float>(9.8504012146370434e-01), static_cast<Float>(7.9029849053031276e-01)
    , static_cast<Float>(5.6082198617463974e-01), static_cast<Float>(3.3133458513996528e-01)
    , static_cast<Float>(1.3692410840839175e-01), static_cast<Float>(1.8914906559664151e-02)
    , static_cast<Float>(-5.1129770932550889e-06), static_cast<Float>(-4.2395493167891873e-04)
    , static_cast<Float>(-4.1934593101534273e-04), static_cast<Float>(1.7473028136486615e-03)
    , static_cast<Float>(3.7999160177631316e-03), static_cast<Float>(-5.5101474906588642e-04)
    , static_cast<Float>(-4.3716662898480967e-05), static_cast<Float>(7.5874501748732798e-03)
    , static_cast<Float>(2.5795650780554021e-02), static_cast<Float>(3.8168376532500548e-02)
    , static_cast<Float>(4.9489586408030833e-02), static_cast<Float>(4.9595992290102905e-02)
    , static_cast<Float>(4.9814819505812249e-02), static_cast<Float>(3.9840911064978023e-02)
    , static_cast<Float>(3.0501024937233868e-02), static_cast<Float>(2.1243054765241080e-02)
    , static_cast<Float>(6.9596532104356399e-03), static_cast<Float>(4.1733649330980525e-03)};

const Float RGB_ILLUM_2_SPECT_WHITE[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.1565232050369776e+00), static_cast<Float>(1.1567225000119139e+00)
    , static_cast<Float>(1.1566203150243823e+00), static_cast<Float>(1.1555782088080084e+00)
    , static_cast<Float>(1.1562175509215700e+00), static_cast<Float>(1.1567674012207332e+00)
    , static_cast<Float>(1.1568023194808630e+00), static_cast<Float>(1.1567677445485520e+00)
    , static_cast<Float>(1.1563563182952830e+00), static_cast<Float>(1.1567054702510189e+00)
    , static_cast<Float>(1.1565134139372772e+00), static_cast<Float>(1.1564336176499312e+00)
    , static_cast<Float>(1.1568023181530034e+00), static_cast<Float>(1.1473147688514642e+00)
    , static_cast<Float>(1.1339317140561065e+00), static_cast<Float>(1.1293876490671435e+00)
    , static_cast<Float>(1.1290515328639648e+00), static_cast<Float>(1.0504864823782283e+00)
    , static_cast<Float>(1.0459696042230884e+00), static_cast<Float>(9.9366687168595691e-01)
    , static_cast<Float>(9.5601669265393940e-01), static_cast<Float>(9.2467482033511805e-01)
    , static_cast<Float>(9.1499944702051761e-01), static_cast<Float>(8.9939467658453465e-01)
    , static_cast<Float>(8.9542520751331112e-01), static_cast<Float>(8.8870566693814745e-01)
    , static_cast<Float>(8.8222843814228114e-01), static_cast<Float>(8.7998311373826676e-01)
    , static_cast<Float>(8.7635244612244578e-01), static_cast<Float>(8.8000368331709111e-01)
    , static_cast<Float>(8.8065665428441120e-01), static_cast<Float>(8.8304706460276905e-01)};

const Float RGB_ILLUM_2_SPECT_CYAN[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.1334479663682135e+00), static_cast<Float>(1.1266762330194116e+00)
    , static_cast<Float>(1.1346827504710164e+00), static_cast<Float>(1.1357395805744794e+00)
    , static_cast<Float>(1.1356371830149636e+00), static_cast<Float>(1.1361152989346193e+00)
    , static_cast<Float>(1.1362179057706772e+00), static_cast<Float>(1.1364819652587022e+00)
    , static_cast<Float>(1.1355107110714324e+00), static_cast<Float>(1.1364060941199556e+00)
    , static_cast<Float>(1.1360363621722465e+00), static_cast<Float>(1.1360122641141395e+00)
    , static_cast<Float>(1.1354266882467030e+00), static_cast<Float>(1.1363099407179136e+00)
    , static_cast<Float>(1.1355450412632506e+00), static_cast<Float>(1.1353732327376378e+00)
    , static_cast<Float>(1.1349496420726002e+00), static_cast<Float>(1.1111113947168556e+00)
    , static_cast<Float>(9.0598740429727143e-01), static_cast<Float>(6.1160780787465330e-01)
    , static_cast<Float>(2.9539752170999634e-01), static_cast<Float>(9.5954200671150097e-02)
    , static_cast<Float>(-1.1650792030826267e-02), static_cast<Float>(-1.2144633073395025e-02)
    , static_cast<Float>(-1.1148167569748318e-02), static_cast<Float>(-1.1997606668458151e-02)
    , static_cast<Float>(-5.0506855475394852e-03), static_cast<Float>(-7.9982745819542154e-03)
    , static_cast<Float>(-9.4722817708236418e-03), static_cast<Float>(-5.5329541006658815e-03)
    , static_cast<Float>(-4.5428914028274488e-03), static_cast<Float>(-1.2541015360921132e-02)};

const Float RGB_ILLUM_2_SPECT_MAGENTA[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.0371892935878366e+00), static_cast<Float>(1.0587542891035364e+00)
    , static_cast<Float>(1.0767271213688903e+00), static_cast<Float>(1.0762706844110288e+00)
    , static_cast<Float>(1.0795289105258212e+00), static_cast<Float>(1.0743644742950074e+00)
    , static_cast<Float>(1.0727028691194342e+00), static_cast<Float>(1.0732447452056488e+00)
    , static_cast<Float>(1.0823760816041414e+00), static_cast<Float>(1.0840545681409282e+00)
    , static_cast<Float>(9.5607567526306658e-01), static_cast<Float>(5.5197896855064665e-01)
    , static_cast<Float>(8.4191094887247575e-02), static_cast<Float>(8.7940070557041006e-05)
    , static_cast<Float>(-2.3086408335071251e-03), static_cast<Float>(-1.1248136628651192e-03)
    , static_cast<Float>(-7.7297612754989586e-11), static_cast<Float>(-2.7270769006770834e-04)
    , static_cast<Float>(1.4466473094035592e-02), static_cast<Float>(2.5883116027169478e-01)
    , static_cast<Float>(5.2907999827566732e-01), static_cast<Float>(9.0966624097105164e-01)
    , static_cast<Float>(1.0690571327307956e+00), static_cast<Float>(1.0887326064796272e+00)
    , static_cast<Float>(1.0637622289511852e+00), static_cast<Float>(1.0201812918094260e+00)
    , static_cast<Float>(1.0262196688979945e+00), static_cast<Float>(1.0783085560613190e+00)
    , static_cast<Float>(9.8333849623218872e-01), static_cast<Float>(1.0707246342802621e+00)
    , static_cast<Float>(1.0634247770423768e+00), static_cast<Float>(1.0150875475729566e+00)};

const Float RGB_ILLUM_2_SPECT_YELLOW[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(2.7756958965811972e-03), static_cast<Float>(3.9673820990646612e-03)
    , static_cast<Float>(-1.4606936788606750e-04), static_cast<Float>(3.6198394557748065e-04)
    , static_cast<Float>(-2.5819258699309733e-04), static_cast<Float>(-5.0133191628082274e-05)
    , static_cast<Float>(-2.4437242866157116e-04), static_cast<Float>(-7.8061419948038946e-05)
    , static_cast<Float>(4.9690301207540921e-02), static_cast<Float>(4.8515973574763166e-01)
    , static_cast<Float>(1.0295725854360589e+00), static_cast<Float>(1.0333210878457741e+00)
    , static_cast<Float>(1.0368102644026933e+00), static_cast<Float>(1.0364884018886333e+00)
    , static_cast<Float>(1.0365427939411784e+00), static_cast<Float>(1.0368595402854539e+00)
    , static_cast<Float>(1.0365645405660555e+00), static_cast<Float>(1.0363938240707142e+00)
    , static_cast<Float>(1.0367205578770746e+00), static_cast<Float>(1.0365239329446050e+00)
    , static_cast<Float>(1.0361531226427443e+00), static_cast<Float>(1.0348785007827348e+00)
    , static_cast<Float>(1.0042729660717318e+00), static_cast<Float>(8.4218486432354278e-01)
    , static_cast<Float>(7.3759394894801567e-01), static_cast<Float>(6.5853154500294642e-01)
    , static_cast<Float>(6.0531682444066282e-01), static_cast<Float>(5.9549794132420741e-01)
    , static_cast<Float>(5.9419261278443136e-01), static_cast<Float>(5.6517682326634266e-01)
    , static_cast<Float>(5.6061186014968556e-01), static_cast<Float>(5.8228610381018719e-01)};

const Float RGB_ILLUM_2_SPECT_RED[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(5.4711187157291841e-02), static_cast<Float>(5.5609066498303397e-02)
    , static_cast<Float>(6.0755873790918236e-02), static_cast<Float>(5.6232948615962369e-02)
    , static_cast<Float>(4.6169940535708678e-02), static_cast<Float>(3.8012808167818095e-02)
    , static_cast<Float>(2.4424225756670338e-02), static_cast<Float>(3.8983580581592181e-03)
    , static_cast<Float>(-5.6082252172734437e-04), static_cast<Float>(9.6493871255194652e-04)
    , static_cast<Float>(3.7341198051510371e-04), static_cast<Float>(-4.3367389093135200e-04)
    , static_cast<Float>(-9.3533962256892034e-05), static_cast<Float>(-1.2354967412842033e-04)
    , static_cast<Float>(-1.4524548081687461e-04), static_cast<Float>(-2.0047691915543731e-04)
    , static_cast<Float>(-4.9938587694693670e-04), static_cast<Float>(2.7255083540032476e-02)
    , static_cast<Float>(1.6067405906297061e-01), static_cast<Float>(3.5069788873150953e-01)
    , static_cast<Float>(5.7357465538418961e-01), static_cast<Float>(7.6392091890718949e-01)
    , static_cast<Float>(8.9144466740381523e-01), static_cast<Float>(9.6394609909574891e-01)
    , static_cast<Float>(9.8879464276016282e-01), static_cast<Float>(9.9897449966227203e-01)
    , static_cast<Float>(9.8605140403564162e-01), static_cast<Float>(9.9532502805345202e-01)
    , static_cast<Float>(9.7433478377305371e-01), static_cast<Float>(9.9134364616871407e-01)
    , static_cast<Float>(9.8866287772174755e-01), static_cast<Float>(9.9713856089735531e-01)};

const Float RGB_ILLUM_2_SPECT_GREEN[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(2.5168388755514630e-02), static_cast<Float>(3.9427438169423720e-02)
    , static_cast<Float>(6.2059571596425793e-03), static_cast<Float>(7.1120859807429554e-03)
    , static_cast<Float>(2.1760044649139429e-04), static_cast<Float>(7.3271839984290210e-12)
    , static_cast<Float>(-2.1623066217181700e-02), static_cast<Float>(1.5670209409407512e-02)
    , static_cast<Float>(2.8019603188636222e-03), static_cast<Float>(3.2494773799897647e-01)
    , static_cast<Float>(1.0164917292316602e+00), static_cast<Float>(1.0329476657890369e+00)
    , static_cast<Float>(1.0321586962991549e+00), static_cast<Float>(1.0358667411948619e+00)
    , static_cast<Float>(1.0151235476834941e+00), static_cast<Float>(1.0338076690093119e+00)
    , static_cast<Float>(1.0371372378155013e+00), static_cast<Float>(1.0361377027692558e+00)
    , static_cast<Float>(1.0229822432557210e+00), static_cast<Float>(9.6910327335652324e-01)
    , static_cast<Float>(-5.1785923899878572e-03), static_cast<Float>(1.1131261971061429e-03)
    , static_cast<Float>(6.6675503033011771e-03), static_cast<Float>(7.4024315686001957e-04)
    , static_cast<Float>(2.1591567633473925e-02), static_cast<Float>(5.1481620056217231e-03)
    , static_cast<Float>(1.4561928645728216e-03), static_cast<Float>(1.6414511045291513e-04)
    , static_cast<Float>(-6.4630764968453287e-03), static_cast<Float>(1.0250854718507939e-02)
    , static_cast<Float>(4.2387394733956134e-02), static_cast<Float>(2.1252716926861620e-02)};

const Float RGB_ILLUM_2_SPECT_BLUE[RGB_2_SPECT_SAMPLES_NUMBER] = {
    static_cast<Float>(1.0570490759328752e+00), static_cast<Float>(1.0538466912851301e+00)
    , static_cast<Float>(1.0550494258140670e+00), static_cast<Float>(1.0530407754701832e+00)
    , static_cast<Float>(1.0579930596460185e+00), static_cast<Float>(1.0578439494812371e+00)
    , static_cast<Float>(1.0583132387180239e+00), static_cast<Float>(1.0579712943137616e+00)
    , static_cast<Float>(1.0561884233578465e+00), static_cast<Float>(1.0571399285426490e+00)
    , static_cast<Float>(1.0425795187752152e+00), static_cast<Float>(3.2603084374056102e-01)
    , static_cast<Float>(-1.9255628442412243e-03), static_cast<Float>(-1.2959221137046478e-03)
    , static_cast<Float>(-1.4357356276938696e-03), static_cast<Float>(-1.2963697250337886e-03)
    , static_cast<Float>(-1.9227081162373899e-03), static_cast<Float>(1.2621152526221778e-03)
    , static_cast<Float>(-1.6095249003578276e-03), static_cast<Float>(-1.3029983817879568e-03)
    , static_cast<Float>(-1.7666600873954916e-03), static_cast<Float>(-1.2325281140280050e-03)
    , static_cast<Float>(1.0316809673254932e-02), static_cast<Float>(3.1284512648354357e-02)
    , static_cast<Float>(8.8773879881746481e-02), static_cast<Float>(1.3873621740236541e-01)
    , static_cast<Float>(1.5535067531939065e-01), static_cast<Float>(1.4878477178237029e-01)
    , static_cast<Float>(1.6624255403475907e-01), static_cast<Float>(1.6997613960634927e-01)
    , static_cast<Float>(1.5769743995852967e-01), static_cast<Float>(1.9069090525482305e-01)};


bool SpectrumSamplesSorted(const Float *lambda, const Float * /*vals*/, int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        if (lambda[i] > lambda[i + 1]) return false;
    }

    return true;
}

// TODO
void SortSpectrumSamples(Float *lambda, Float *vals, int n)
{
    std::vector<std::pair<Float, Float>> sort_vec;
    sort_vec.reserve(n);
    for (int i = 0; i < n; ++i)
    {
        sort_vec.push_back(std::make_pair(lambda[i], vals[i]));
    }
    std::sort(sort_vec.begin(), sort_vec.end());
    for (int i = 0; i < n; ++i)
    {
        lambda[i] = sort_vec[i].first;
        vals[i] = sort_vec[i].second;
    }
}

// TODO
Float AverageSpectrumSamples(const Float *lambda, const Float *vals, int n,
    Float lambdaStart, Float lambdaEnd)
{
#ifdef DEBUG
    for (int i = 0; i < n - 1; ++i)
    {
        CHECK_GT(lambda[i + 1], lambda[i]);
    }
    CHECK_LT(lambdaStart, lambdaEnd);
#endif // DEBUG
    // Handle cases with out-of-bounds range or single sample only
    if (lambdaEnd <= lambda[0]) return vals[0];
    if (lambdaStart >= lambda[n - 1]) return vals[n - 1];
    if (n == 1) return vals[0];
    Float sum = FLOAT_0;
    // Add contributions of constant segments before/after samples
    if (lambdaStart < lambda[0]) sum += vals[0] * (lambda[0] - lambdaStart);
    if (lambdaEnd > lambda[n - 1])
        sum += vals[n - 1] * (lambdaEnd - lambda[n - 1]);

    // Advance to first relevant wavelength segment
    int i = 0;
    for (; lambdaStart > lambda[i + 1]; ++i);
#ifdef DEBUG
    CHECK_LT(i + 1, n);
#endif // DEBUG

    // Loop over wavelength sample segments and add contributions
    auto interp = [lambda, vals](Float w, int i)
    {
        return common::math::Lerp((w - lambda[i]) / (lambda[i + 1] - lambda[i]), vals[i],
            vals[i + 1]);
    };
    for (; i + 1 < n && lambdaEnd >= lambda[i]; ++i)
    {
        Float segLambdaStart = (std::max)(lambdaStart, lambda[i]);
        Float segLambdaEnd = (std::min)(lambdaEnd, lambda[i + 1]);
        sum += FLOAT_INV_2 * (interp(segLambdaStart, i) + interp(segLambdaEnd, i)) *
            (segLambdaEnd - segLambdaStart);
    }
    return sum / (lambdaEnd - lambdaStart);
}

Float InterpolateSpectrumSamples(const Float *lambda, const Float *vals, int n,
    Float l)
{
#ifdef DEBUG
    for (int i = 0; i < n - 1; ++i)
    {
        CHECK_GT(lambda[i + 1], lambda[i]);
    }
#endif
    if (l <= lambda[0]) return vals[0];
    if (l >= lambda[n - 1]) return vals[n - 1];
    int offset = 0;
    common::math::FindInterval(n, [&](int index)
    {
        return lambda[index] <= l;
    });
#ifdef DEBUG
    CHECK(l >= lambda[offset] && l <= lambda[offset + 1]);
#endif
    Float t = (l - lambda[offset]) / (lambda[offset + 1] - lambda[offset]);
    return common::math::Lerp(t, vals[offset], vals[offset + 1]);
}


}
}