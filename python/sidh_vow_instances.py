# Definition of instances to run attack on SIDH

insts_stats = {
    'p_32_20': [{
        "MODULUS": "p_32_20",
        "NBITS_K": 15,
        "MEMORY_LOG_SIZE": 9,
        "ALPHA": 2.25,
        "BETA": 10.,
        "GAMMA": 20.,
        "PRNG_SEED": 1337,
        "jinv": [0x87D581FB16D011BD, 0x3, 0x5D14A016E29F3C, 0x1],
        "E": [
            {
                "a24": [0x898F502EC74EFE54, 0x3, 0x649BFBDDF471F7D0, 0x4],
                "xp":  [0x81B68BDCA5EEE933, 0x1, 0x9BC03274CCE5407C, 0xE],
                "xq":  [0x82D38AA521A411ED, 0x7, 0x8ECFD9A4B8E874AE, 0x6],
                "xpq": [0xA7DEE573517AA282, 0x5, 0x6AAA88DC2F3041ED, 0xA],
            },
            {
                "a24": [0x5219884293D7ACA9, 0x3, 0xCCB60983643DBA16, 0xF],
                "xp":  [0x34C2A2B46B60718A, 0xF, 0x7066C62B26701E69, 0xE],
                "xq":  [0x13E5C49616025EB1, 0x11, 0xD4D573F7A00911FA, 0x4],
                "xpq": [0x3019C8E745061408, 0xE, 0x349BC9BDE78E1278, 0x7],
            }]
    }],
    'p_36_22':
        [{
            "MODULUS": "p_36_22",
            "NBITS_K": 17,
            "MEMORY_LOG_SIZE": 10,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0x87A382029340BEE1, 0xB2C, 0xD365301600BEECF4, 0x988],
                "E": [
                {
                    "a24": [0x5A29737102A81928, 0x8BA, 0xB36DA01007472BCA, 0x3B0],
                    "xp":  [0x319E9FB3E22FB3EE, 0xDFF, 0xD8CF9A346BD3855F, 0x649],
                    "xq":  [0xDBCE4A4D3510A09B, 0xBA2, 0x7BF842B88B390A1D, 0x289],
                    "xpq": [0x924B27056686B1EA, 0xAD8, 0x9E4C37581B001A98, 0x92F],
                },
                {
                    "a24": [0x3E579DB11FB4DA49, 0x72A, 0x4699A6E3C970136F, 0x2A5],
                    "xp":  [0x291480E9DC649F7B, 0x8B4, 0x5004FA5CE3FECE00, 0x624],
                    "xq":  [0x434891C342FD5996, 0xB56, 0xB6EC2618488333D, 0x2B6],
                    "xpq": [0x79BB8267FA1C4EA6, 0x8A6, 0xEEC22455EED1A511, 0x105],
                }]
        }],
    'p_40_25':
        [{
            "MODULUS": "p_40_25",
            "NBITS_K": 19,
            "MEMORY_LOG_SIZE": 11,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0xF75F97ACCCE7FDFA, 0x1B8DF, 0xBA82825D5C701E61, 0x2DAEE9],
                "E": [
                {
                    "a24": [0x1CC38F4019578E81, 0x2B899F, 0xCF013F0B3F81DDD6, 0x358896],
                    "xp":  [0xCD555DFA114727B, 0x32D06D, 0xC3877F11ADEF4F6D, 0x10E535],
                    "xq":  [0x35DFE009C86FB00, 0x10B5EF, 0x10FD65390B9F1892, 0x2EC958],
                    "xpq": [0xDECC4D01E3930FF, 0x2A199B, 0x4497C1DC5FF2BEBE, 0x8FF1F],
                },
                {
                    "a24": [0x4A73A52B7C4F799C, 0x276F98, 0xF0391AC98A62567A, 0x1E0E3A],
                    "xp":  [0x96166625DDE0E82D, 0x128FF2, 0xB8B2FB529A8705A4, 0x28899B],
                    "xq":  [0x148106395B4403E8, 0x17D5DD, 0x881D0462B88875E9, 0x142BEF],
                    "xpq": [0x92BC55E7309AD736, 0x2CE71C, 0x643B765FE674EA1A, 0x2CA812],
                }]
        }],
    'p_44_27':
        [{
            "MODULUS": "p_44_27",
            "NBITS_K": 21,
            "MEMORY_LOG_SIZE": 13,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0x86206DFCAA0A264A, 0x78572E8, 0x39270047F235C429, 0xA31ABCE],
                "E": [
                {
                    "a24": [0xBD1D693E2568E84D, 0x5B50EB8, 0x1041FC5BA2D18488, 0xAB98BC7],
                    "xp":  [0x5239F713E8A33366, 0x6DCFE8, 0x3FB340C77BCAB3BB, 0x53184A7],
                    "xq":  [0xCB08294215E1F044, 0xE47BD26, 0x44A1181BB566FD68, 0x5438288],
                    "xpq": [0xEB5930FB33C5143D, 0xA1EC91D, 0x121A958939601A4C, 0xFD26A6D],
                },
                {
                    "a24": [0xE471F64DC6C2C0DB, 0xF7E04ED, 0xAA3E817477E38CFC, 0xDD1A5D],
                    "xp":  [0x1940779119BBE841, 0x99DAD79, 0x42527FF3B867F3E7, 0xB1D46D4],
                    "xq":  [0x49C69459160701C9, 0xEEE4BDD, 0xBCBC3AD71E96533, 0x14D9570],
                    "xpq": [0x4699321466FFFED, 0x78A07DD, 0x434217C992D1BD22, 0xDA8D784],
                }]
        }],
    'p_48_30':
        [{
            "MODULUS": "p_48_30",
            "NBITS_K": 23,
            "MEMORY_LOG_SIZE": 13,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0x960D5A87B64308AB, 0x20A35A461, 0x7523472421121368, 0x9386EABEF],
                "E": [
                {
                "a24": [0x3B4F8D170350D548, 0xE2377BCD, 0x650353EBFBFF26EE, 0x7F42E3B5],
                    "xp":  [0x58FFA9C6202147C5, 0x17FA73C2D, 0xC0FDFF1F804A427B, 0x14290A002],
                    "xq":  [0x4ED73B68BB0D3B9A, 0x33805821D, 0x625B8F38A22A44DA, 0x3BA92AFBB],
                    "xpq": [0x6EFCC4C4A47DD803, 0x1279F463D, 0xE10F37920B36744, 0x92D79D18E],
                },
                {
                "a24": [0xE0C860256BCD6FEE, 0x925CEEEA1, 0xE3937DD0DEB0514, 0x1E2CD43C4],
                    "xp":  [0x91DF3C61FDBBAE51, 0x7179DBCEF, 0x93634FFDC2FA637B, 0x5A95844B3],
                    "xq":  [0xFB6D71FB622FF503, 0x465C692DB, 0xE81874E6C2492D3E, 0x379850F18],
                    "xpq": [0xA0A5A0C523139CE2, 0xD8DB9A27, 0xFBB158570656A25B, 0x23F5AF6F],
                }]
        }],
    'p_52_33':
        [{
            "MODULUS": "p_52_33",
            "NBITS_K": 25,
            "MEMORY_LOG_SIZE": 15,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0xD1606390392C7791, 0x3FA806AA4D, 0x2563EC5EF4D1F38A, 0xA14E008C29],
                "E": [
                {
                "a24": [0x25C1EA4E4FC96D2D, 0xE26FC6EF98, 0x60A41C936A22C300, 0x2B933B12E3],
                    "xp":  [0x63476390E5A241FD, 0x81B793B23, 0x6000B176727CFEA7, 0x2A0C0310D8],
                    "xq":  [0x4A3ECB2550CA8B2B, 0xF117F0449B, 0x112F3937BA02F014, 0x10ECB394AEE],
                    "xpq": [0x289167DBB29A9A17, 0x9BBFB98980, 0xF2EFE916AAF82722, 0x117501F1348],
                },
                {
                "a24": [0x5C507654830DB7C9, 0xC8812E429C, 0x8F179FBB7C78B1C, 0xBA853B961D],
                    "xp":  [0xFAC4455A295157BB, 0x7FF471E27F, 0x2C081E773CF01D9C, 0x9B3A597D07],
                    "xq":  [0xF8B1EB38C80BF1B1, 0x10101371490, 0x394D5EE280E0CFCC, 0x3FB3D830EA],
                    "xpq": [0x863280D10852C58, 0x1052B0FB803, 0xD98096C33EA19F7C, 0x110393B3545],
                }]
        }],
    'p_56_35':
        [{
            "MODULUS": "p_56_35",
            "NBITS_K": 27,
            "MEMORY_LOG_SIZE": 17,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0xF6F347F9032A0107, 0x12B845E5EBF9D6, 0x9E1E48E0FB6AB1C0, 0x13F3DB314AD742],
                "E": [
                {
                "a24": [0x3B2532BA9EA6DAAC, 0x160224D27B926B, 0x154C3A07FE6D299A, 0xFA38BA4AC0302],
                    "xp":  [0xC7ED3D15A04261CA, 0x24812E86F7AD9E, 0xB8510AB4F0614E56, 0x12A740ACACDCD8],
                    "xq":  [0x2D71FCC4FC0BD439, 0x2592F05EF2C25F, 0xA8D05DB3D60D96C3, 0x1AE72A62526B76],
                    "xpq": [0x76FAF00968A8A38, 0x1597243B6E86F8, 0xD715B9A7E1D968D4, 0x1A75522A1E1C7],
                },
                {
                "a24": [0xCD38EB0CCA8305CD, 0xD57B6A098285A, 0xDF77A04A9754372F, 0xDB0574569EB95],
                    "xp":  [0x60E6D22294364FA8, 0x26F30FA4223D71, 0xF9C30E9D361D036C, 0xCC128C07E3F41],
                    "xq":  [0x80958AEF10D5F48C, 0x26DEC67CE7C1F2, 0xCCC1250DE5B96630, 0x9DCF617F771B7],
                    "xpq": [0x708903C4A04938F, 0x80EF65C2C32FA, 0x2D9954213EF20C96, 0x18E0B6E4A7FD3E],
                }]
        }],
    'p_60_38':
        [{
            "MODULUS": "p_60_38",
            "NBITS_K": 29,
            "MEMORY_LOG_SIZE": 19,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [0x8B7516529AB197CB, 0x81A9A4D07F7A660, 0x714920C82BDA2001, 0xC547C48B9C3A244],
                "E": [
                {
                "a24": [0xF930648E75B7B02, 0x1CBBCCC43606B060, 0xA0774309743DD761, 0x3A1E5A234C177637],
                    "xp":  [0x1E39EABFBA272D88, 0x1C8B8F8C4CBF5E2B, 0xE32FF4D09B5C55F3, 0x326AFBF252FCC890],
                    "xq":  [0xF6989AED51FACE6C, 0x30E9B909787463EE, 0x932757D32E4C2386, 0x14CA8E398521792A],
                    "xpq": [0xB0C0702B028E73EA, 0x1A9EB9332A0E2B49, 0xAD8E41C6323A7187, 0x39A70907266DF35E],
                },
                {
                "a24": [0xE501F1498BFD9C5F, 0x4213F826FBA1CD2E, 0xF8FAB7BDF548DEF4, 0x15B5AE4150593F58],
                    "xp":  [0x6AA8833297C80EDD, 0x36775CB8AD388526, 0x5909C1F233929A0F, 0x1E155B7EAF673DBC],
                    "xq":  [0xFDE3E4BDB7A49C2A, 0xAF5506745D90CC4, 0x34C4DCA1A2DC0101, 0x2D5D0DE11D479AD7],
                    "xpq": [0xFE9411686F5DBA9A, 0xDCCA32ABE1FDAC9, 0x78347535B8DBF7C1, 0x30C5AE7FF6AC7BB4],
                }]
        }],
    'p_216_137':
        [{
            "MODULUS": "p_216_137",
            "NBITS_K": 19,
            "MEMORY_LOG_SIZE": 10,
            "ALPHA": 2.25,
            "BETA": 10.,
            "GAMMA": 20.,
            "PRNG_SEED": 1337,
            "jinv": [ 0xA5F257905654FB15, 0xB6573F76CE3F5F68, 0xB45F6B58CBE582D2, 0x3854AF2673AF53DC, 0x635BE68F3A3CFB45, 0xBB289DD0FABB82B9, 0x1A6A91983AE18, 0x2A22643D6E60B7C8, 0x636AA2F513A44D17, 0x897523D2A01F3A5C, 0x54BF81722052C120, 0x66FBB3A5FCE700F2, 0xBB6A5A4B8EB4CC9F, 0x189EA53C9E399 ],
                "E": [
                {
                "a24": [ 0xF54D04153E13101E, 0xFC96AE56DE382073, 0xB897333B74492469, 0x25C45BF15735194C, 0xD5227E44D30A40D3, 0x7AEE45461EDA719, 0x1534539164408, 0x6B9E807E638CC22E, 0x8571F85C94159536, 0xDD8C675511142DCE, 0x301E43D2CE92FC2F, 0xFC60D281399BB8E, 0x9A7CC797CF4BE2FF, 0x44875853D59C ],
                "xp": [ 0x40A1FCDACD77BC2C, 0x6B92116358160920, 0xA6FC804B62C587F5, 0x870FBE633E948B54, 0xFD231FCDB226B7F, 0x649B38D037E41BDB, 0x14815B6B09C80, 0xCE70F8CD8FBBD870, 0xA122A3667E0A0D10, 0xF680995D18B9C26D, 0x1352A48569092414, 0x7AEE9CFD9D44A980, 0xA410DF22A99E022C, 0x31F3290807E8 ],
                "xq": [ 0x5FE9F76587114263, 0x16E17E2D2C13907A, 0xD33046004ADE11E6, 0xA91702C0B8FBCF19, 0x9DE090D7F533403, 0x9D8209318FD7C6DA, 0x12A49637BCAA2, 0xB10FE56F1815AFF, 0x406760FDBB424734, 0xB8CE7126545A9773, 0x2ACA3EC15B0A141A, 0x3EECB49B30043724, 0x6E107E6453318AEE, 0xC63227F1A282 ],
                "xpq": [ 0x8C7446336B2A87E2, 0x20AF31E8B8F49B0C, 0x2E6CB236D990F81B, 0x2C27F7C1D256EBA4, 0xF9E19A50ED1887F1, 0x9C5F212FFD5F139, 0x64537E43860C, 0x8E3542454BB71B08, 0x4B6EE567F1D87769, 0x9FFF61D5AF1E7E56, 0xF01F0462448B762A, 0xF1687D21EBC9BFBE, 0xF266085593778200, 0x2198F4CFA150F ],
                },
                {
                "a24": [ 0x29BBA6C05A034249, 0x21FA4B27D9665C70, 0x1B75DDE864E86DE, 0x3CC9FC6C2DF337C, 0x7F8112960D44331A, 0x1B98875DEA7745C7, 0x521CD8307497, 0x6DE8442A3AACAF10, 0x36548A217F137913, 0xAE5F12522879DE40, 0x94D3C078B20D0745, 0x3EBAEA6E011703D6, 0x7F596F14CAC85582, 0x20EDF64D07265 ],
                "xp": [ 0xC6F28DCC478A1854, 0x9527F0C675449E55, 0x8124CCE8FEF1BE81, 0xA3533C55F25538E4, 0xC14B0719718745D9, 0x858009F71EE0DDEA, 0x20C20A9E3195, 0xACF1D9CA0EA181F6, 0xA0936A3B71ECE3B4, 0xAB0D86558870ED02, 0x52BB113AE1068D99, 0xD838F3A618FDFEF8, 0x254C671EBB4D0F58, 0x1FBD0A7E4B9B1 ],
                "xq": [ 0xC34F71E2C9AE113E, 0x488BB0C210365454, 0x3823FA635130E1E6, 0x73B9811827524228, 0x367C78D933BAC757, 0xCF2E1A4C54DF78E3, 0x195CA2FFFF5E7, 0x6358E27F9CE78576, 0x7A167C0B531C53E0, 0x89B42D14467DE6A7, 0x38596D6AA9B67BC0, 0xE46C3740C4F2A1B, 0x50818B24419D8D30, 0x79AC9F68A0DC ],
                "xpq": [ 0x88B298D906E130E9, 0xFD81720096DF7D3E, 0xE49A435CD998F644, 0x4DFC6581487B4CDE, 0xCFADB824C9BD3918, 0xA4B425300D6AC3FF, 0x237444A7B417, 0x35FFFFAA785DB1B8, 0x16C7B61F24B1D187, 0x5D3488E806EB9525, 0xA475BAB2D2D35181, 0x5CF94085E8C71225, 0xAE22B440DCBDD9F8, 0xBF38A5857B06 ],
                }]
        }],
    }


def load_instance(modulus):
    if modulus not in insts_stats:
        keys = insts_stats.keys()
        keys.sort()
        raise ValueError("Only available moduli are %s" % ", ".join(keys))
    
    import vow_sidh.sidh_vow as sidh_vow
    ret = []
    insts = insts_stats[modulus]
    for inst in insts:
        t = sidh_vow.instance_t()
        t.MODULUS = inst["MODULUS"]
        t.NBITS_K = inst["NBITS_K"]
        t.MEMORY_LOG_SIZE = inst["MEMORY_LOG_SIZE"]
        t.ALPHA = inst["ALPHA"]
        t.BETA = inst["BETA"]
        t.GAMMA = inst["GAMMA"]
        t.PRNG_SEED = inst["PRNG_SEED"]
        sidh_vow.load_f2elm(t.jinv, inst["jinv"])

        e0 = sidh_vow.CurveAndPointsSIDH()
        sidh_vow.load_f2elm(e0.a24,inst["E"][0]["a24"])
        sidh_vow.load_f2elm(e0.xp, inst["E"][0]["xp"])
        sidh_vow.load_f2elm(e0.xq, inst["E"][0]["xq"])
        sidh_vow.load_f2elm(e0.xpq,inst["E"][0]["xpq"])
        
        e1 = sidh_vow.CurveAndPointsSIDH()
        sidh_vow.load_f2elm(e1.a24,inst["E"][1]["a24"])
        sidh_vow.load_f2elm(e1.xp, inst["E"][1]["xp"])
        sidh_vow.load_f2elm(e1.xq, inst["E"][1]["xq"])
        sidh_vow.load_f2elm(e1.xpq,inst["E"][1]["xpq"])
        
        sidh_vow.load_E(t, e0, e1)
        ret.append(t)

    return ret
