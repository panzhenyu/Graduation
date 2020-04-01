//需要测量的事件
////////////////////////////////////////////////////////////////////////////////////
static uint64_t benchmark_events_id[4] = {PERF_COUNT_HW_INSTRUCTIONS, PERF_COUNT_HW_CPU_CYCLES, PERF_COUNT_HW_CACHE_MISSES, PERF_COUNT_HW_CACHE_REFERENCES};
static char *benchmark_events_name[4] = {"PERF_COUNT_HW_INSTRUCTIONS", "PERF_COUNT_HW_CPU_CYCLES", "PERF_COUNT_HW_CACHE_MISSES", "PERF_COUNT_HW_CACHE_REFERENCES"};
////////////////////////////////////////////////////////////////////////////////////

//执行环境数据
//*********************************************************************************************************************
char *env508[] = {"namd_r_base.mytest-m64", "--input", "apoa1.input", "--output", "apoa1.ref.output", "--iterations", "65", NULL};
//  ./parest_r_base.mytest-m64 ref.prm
char *env510[] = {"parest_r_base.mytest-m64", "ref.prm", NULL};
// /povray_r_base.mytest-m64 SPEC-benchmark-ref.ini
char *env511[] = {"povray_r_base.mytest-m64", "SPEC-benchmark-ref.ini", NULL};
// /lbm_r_base.mytest-m64 3000 reference.dat 0 0 100_100_130_ldc.of
char *env519[] = {"lbm_r_base.mytest-m64", "3000", "reference.dat", "0", "0", "100_100_130_ldc.of", NULL};
// /blender_r_base.mytest-m64 sh3_no_char.blend --render-output sh3_no_char_ --threads 1 -b -F RAWTGA -s 849 -e 849 -a
char *env526[] = {"blender_r_base.mytest-m64", "sh3_no_char.blend", "--render-output", "sh3_no_char_",
                  "--threads", "1", "-b", "-F", "RAWTGA", "-s", "849", "-e", "849", "-a", NULL};
// /imagick_r_base.mytest-m64 -limit disk 0 refrate_input.tga -edge 41 -resample 181% -emboss 31 -colorspace YUV -mean-shift 19x19+15% -resize 30% refrate_output.tga?
char *env538[] = {"imagick_r_base.mytest-m64", "-limit", "disk", "0", "refrate_input.tga", "-edge", "41", "-resample",
                  "181%", "-emboss", "31", "-colorspace", "YUV", "-mean-shift", "19x19+15%", "-resize", "30%", "refrate_output.tga", NULL};
// /nab_r_base.mytest-m64 1am0 1122214447 122 > 1am0.out 2
char *env544[] = {"nab_r_base.mytest-m64", "1am0", "1122214447", "122", NULL};
//997 太短 char *env[]={"","","","","","","","","","","","","",NULL};
// /lbm_s_base.mytest-m64 2000 reference.dat 0 0 200_200_260_ldc.of
char *env619[] = {"lbm_s_base.mytest-m64", "2000", "reference.dat", "0", "0", "200_200_260_ldc.of", NULL};
// /imagick_s_base.mytest-m64 -limit disk 0 refspeed_input.tga -resize 817% -rotate -2.76 -shave 540x375 -alpha remove -auto-level -contrast-stretch 1x1% -colorspace Lab -channel R -equalize +channel -colorspace sRGB -define histogram:unique-colors=false -adaptive-blur 0x5 -despeckle -auto-gamma -adaptive-sharpen 55 -enhance -brightness-contrast 10x10 -resize 30% refspeed_output.tga > refspeed_convert.out 2
char *env638[] = {"imagick_s_base.mytest-m64", "-limit", "disk", "0", "refspeed_input.tga", "-resize", "817%", "-rotate", "-2.76",
                  "-shave", "540x375", "-alpha", "remove", "-auto-level", "-contrast-stretch", "1x1%", "-colorspace", "Lab", "-channel", "R",
                  "-equalize", "+channel", "-colorspace", "sRGB", "-define", "histogram:unique-colors=false", "-adaptive-blur",
                  "-adaptive-blur", "0x5", "-despeckle", "-auto-gamma", "-adaptive-sharpen", "55", "-enhance", "-brightness-contrast",
                  "10x10", "-resize", "30%", "refspeed_output.tga ", NULL};
// /nab_s_base.mytest-m64 3j1n 20140317 220
char *env644[] = {"nab_s_base.mytest-m64", "3j1n", "20140317", "220", NULL};
//996 太短 char *env[]={"","","","","","","","","","","","","",NULL};
// /perlbench_r_base.mytest-m64 -I./lib checkspam.pl 2500 5 25 11 150 1 1 1 1
char *env500[] = {"perlbench_r_base.mytest-m64", "-I./lib", "checkspam.pl", "2500", "5", "25", "11", "150", "1", "1", "1", "1", NULL};
// /cpugcc_r_base.mytest-m64 gcc-pp.c -O3 -finline-limit=0 -fif-conversion -fif-conversion2 -o gcc-pp.opts-O3_-finline-limit_0_-fif-conversion_-fif-conversion2.s
char *env502[] = {"cpugcc_r_base.mytest-m64", "gcc-pp.c", "-O3", "-finline-limit=0", "-fif-conversion", "-fif-conversion2", "-o",
                  "gcc-pp.opts-O3_-finline-limit_0_-fif-conversion_-fif-conversion2.s", NULL};
// /mcf_r_base.mytest-m64 inp.in
char *env505[] = {"mcf_r_base.mytest-m64", "inp.in", NULL};
// /omnetpp_r_base.mytest-m64 -c General -r 0
char *env520[] = {"omnetpp_r_base.mytest-m64", "-c", "General", "-r", "0", NULL};
// cpuxalan_r_base.mytest-m64 -v t5.xml xalanc.xsl
char *env523[] = {"cpuxalan_r_base.mytest-m64", "-v", "t5.xml", "xalanc.xsl", NULL};
// /x264_r_base.mytest-m64 --pass 1 --stats x264_stats.log --bitrate 1000 --frames 1000 -o BuckBunny_New.264 BuckBunny.yuv 1280x72
char *env525[] = {"x264_r_base.mytest-m64", "--pass", "1", "--stats", "x264_stats.log", "--bitrate", "1000", "--frames", "1000",
                  "-o", "BuckBunny_New.264", "BuckBunny.yuv", "1280x720", NULL};
// /deepsjeng_r_base.mytest-m64 ref.txt
char *env531[] = {"deepsjeng_r_base.mytest-m64", "ref.txt", NULL};
// /leela_r_base.mytest-m64 ref.sgf
char *env541[] = {"leela_r_base.mytest-m64", "ref.sgf", NULL};
// /xz_r_base.mytest-m64 cld.tar.xz 160  59796407 61004416 6
char *env557[] = {"xz_r_base.mytest-m64", "cld.tar.xz", "160",
                  "19cf30ae51eddcbefda78dd06014b4b96281456e078ca7c13e1c0c9e6aaea8dff3efb4ad6b0456697718cede6bd5454852652806a657bb56e07d61128434b474",
                  "59796407", "61004416", "6", NULL};
// 999短
// /perlbench_s_base.mytest-m64 -I./lib checkspam.pl 2500 5 25 11 150 1 1 1 1
char *env600[] = {"perlbench_s_base.mytest-m64", "-I./lib", "checkspam.pl", "2500", "5", "25", "11", "150", "1", "1", "1", "1", NULL};
// /sgcc_base.mytest-m64 gcc-pp.c -O5 -fipa-pta -o gcc-pp.opts-O5_-fipa-pta.s
char *env602[] = {"sgcc_base.mytest-m64", "gcc-pp.c", "-O5", "-fipa-pta", "-o", "gcc-pp.opts-O5_-fipa-pta.s", NULL};
// /mcf_s_base.mytest-m64 inp.in
char *env605[] = {"mcf_s_base.mytest-m64", "inp.in", NULL};
// /omnetpp_s_base.mytest-m64 -c General -r 0
char *env620[] = {"omnetpp_s_base.mytest-m64", "-c", "General", "-r", "0", NULL};
// /xalancbmk_s_base.mytest-m64 -v t5.xml xalanc.xsl
char *env623[] = {"xalancbmk_s_base.mytest-m64", "-v", "t5.xml", "xalanc.xsl", NULL};
//  /x264_s_base.mytest-m64 --pass 1 --stats x264_stats.log --bitrate 1000 --frames 1000 -o BuckBunny_New.264 BuckBunny.yuv 1280x72
char *env625[] = {"x264_s_base.mytest-m64", "--pass", "1", "--stats", "x264_stats.log", "--bitrate", "1000", "--frames", "1000", "-o",
                  "BuckBunny_New.264", "BuckBunny.yuv", "1280x720", NULL};
// /deepsjeng_s_base.mytest-m64 ref.txt
char *env631[] = {"deepsjeng_s_base.mytest-m64", "ref.txt", NULL};
// /leela_s_base.mytest-m64 ref.sgf
char *env641[] = {"leela_s_base.mytest-m64", "ref.sgf", NULL};
// /
char *env657[] = {"xz_s_base.mytest-m64", "cpu2006docs.tar.xz", "6643",
                  "055ce243071129412e9dd0b3b69a21654033a9b723d874b2015c774fac1553d9713be561ca86f74e4f16f22e664fc17a79f30caa5ad2c04fbc447549c2810fae",
                  "1036078272", "1111795472", "4", NULL};
// /bwaves_r_base.mytest-m64 bwaves_1
char *env503[] = {"bwaves_r_base.mytest-m64", "bwaves_1", NULL};
// /speed_bwaves_base.mytest-m64 bwaves_1
char *env603[] = {"speed_bwaves_base.mytest-m64", "bwaves_1", NULL};
// /exchange2_r_base.mytest-m64 6
char *env548[] = {"exchange2_r_base.mytest-m64", "6", NULL};
//  /exchange2_s_base.mytest-m64 6
char *env648[] = {"exchange2_s_base.mytest-m64", "6", NULL};
// /cactusBSSN_r_base.mytest-m64 spec_ref.par
char *env507[] = {"cactusBSSN_r_base.mytest-m64", "spec_ref.par", NULL};
// /cactuBSSN_s_base.mytest-m64 spec_ref.par
char *env607[] = {"cactuBSSN_s_base.mytest-m64", "spec_ref.par", NULL};
// /wrf_r_base.mytest-m64
char *env521[] = {"wrf_r_base.mytest-m64", NULL};
// /wrf_s_base.mytest-m64
char *env621[] = {"wrf_s_base.mytest-m64", NULL};
// /cam4_r_base.mytest-m64 > cam4_r_base.mytest-m64.txt 2
char *env527[] = {"cam4_r_base.mytest-m64", NULL};
// /cam4_s_base.mytest-m64
char *env627[] = {"cam4_s_base.mytest-m64", NULL};
// /fotonik3d_r_base.mytest-m64
char *env549[] = {"fotonik3d_r_base.mytest-m64", NULL};
// fotonik3d_s_base.mytest-m64
char *env649[] = {"fotonik3d_s_base.mytest-m64", NULL};
// /roms_r_base.mytest-m64
char *env554[] = {"roms_r_base.mytest-m64", NULL};
// /sroms_base.mytest-m64
char *env654[] = {"sroms_base.mytest-m64", NULL};
// /speed_pop2_base.mytest-m64
char *env628[] = {"speed_pop2_base.mytest-m64", NULL};  //42

//spec 2006 执行环境数据
//*********************************************************************************************************************
//43
char *env_gromacs[7] = {"./gromacs_base.amd64-m64-gcc42-nn", "-silent", "-deffnm", "gromacs", "-nice", "0", NULL};

//44
char *env_bzip[7] = {"bzip2_base.amd64-m64-gcc42-nn", "chicken.jpg", "30", NULL};

//45
char *env_mcf[7] = {"mcf_base.amd64-m64-gcc42-nn", "inp.in", NULL};

//46
char *env_calculix[7] = {"calculix_base.amd64-m64-gcc42-nn", "-i", /*"stairs""beampic"*/ "hyperviscoplastic", NULL};

//47
char *env_sphinx3[7] = {"sphinx_livepretend_base.amd64-m64-gcc42-nn", "ctlfile", ".", "args.an4", NULL};

//48
char *env_lbm[7] = {"lbm_base.amd64-m64-gcc42-nn", "3000", "reference.dat", "0", "0", "100_100_130_ldc.of", NULL};

//49
char *env_libquantum[7] = {"libquantum_base.amd64-m64-gcc42-nn", "1397", "8", NULL};

//50
char *env_soplex[7] = {"soplex_base.amd64-m64-gcc42-nn", "-m3500", "ref.mps", NULL};

///51
char *env_gcc[7] = {"gcc_base.amd64-m64-gcc42-nn", "s04.i", "-o", "s04.s", NULL};

//52
char *env_dealII[7] = {"dealII_base.amd64-m64-gcc42-nn", "23", NULL};

//53
char *env_povray[7] = {"povray_base.amd64-m64-gcc42-nn", "SPEC-benchmark-ref.ini", NULL};

///54
char *env_omnetpp[7] = {"omnetpp_base.amd64-m64-gcc42-nn", "omnetpp.ini", NULL};
//!!!!!!!!!
//55
char *env_xalancbmk[7] = {"Xalan_base.amd64-m64-gcc42-nn", "-v", "t5.xml", "xalanc.xsl", NULL};

//56
char *env_hmmer[7] = {"hmmer_base.amd64-m64-gcc42-nn", "nph3.hmm", "swiss41", NULL};

char *env_reporter[7]={"reproter",NULL};

const char *bin_table[] = {
    "./namd_r_base.mytest-m64", //0
    "./parest_r_base.mytest-m64",
    "./povray_r_base.mytest-m64",
    "./lbm_r_base.mytest-m64",
    "./blender_r_base.mytest-m64",
    "./imagick_r_base.mytest-m64",
    "./nab_r_base.mytest-m64",
    "./lbm_s_base.mytest-m64",
    "./imagick_s_base.mytest-m64",
    "./nab_s_base.mytest-m64",       //9
    "./perlbench_r_base.mytest-m64", //10
    "./cpugcc_r_base.mytest-m64",
    "./mcf_r_base.mytest-m64",
    "./omnetpp_r_base.mytest-m64",
    "./cpuxalan_r_base.mytest-m64",
    "./x264_r_base.mytest-m64",
    "./deepsjeng_r_base.mytest-m64",
    "./leela_r_base.mytest-m64",
    "./xz_r_base.mytest-m64",
    "./perlbench_s_base.mytest-m64", //19
    "./sgcc_base.mytest-m64",
    "./mcf_s_base.mytest-m64",
    "./omnetpp_s_base.mytest-m64",
    "./xalancbmk_s_base.mytest-m64",
    "./x264_s_base.mytest-m64",
    "./deepsjeng_s_base.mytest-m64",
    "./leela_s_base.mytest-m64",
    "./xz_s_base.mytest-m64",

    "./bwaves_r_base.mytest-m64",
    "./speed_bwaves_base.mytest-m64",
    "./exchange2_r_base.mytest-m64",
    "./exchange2_s_base.mytest-m64",
    "./cactusBSSN_r_base.mytest-m64",
    "./cactuBSSN_s_base.mytest-m64",
    "./wrf_r_base.mytest-m64",
    "./wrf_s_base.mytest-m64",
    "./cam4_r_base.mytest-m64",
    "./cam4_s_base.mytest-m64",
    "./fotonik3d_r_base.mytest-m64",
    "./fotonik3d_s_base.mytest-m64",
    "./roms_r_base.mytest-m64",
    "./sroms_base.mytest-m64",
    "./speed_pop2_base.mytest-m64",
    //spec2006
    "./gromacs_base.amd64-m64-gcc42-nn", "./bzip2_base.amd64-m64-gcc42-nn",
                           "./mcf_base.amd64-m64-gcc42-nn", "./calculix_base.amd64-m64-gcc42-nn",
                           "./sphinx_livepretend_base.amd64-m64-gcc42-nn", "./lbm_base.amd64-m64-gcc42-nn",
                           "./libquantum_base.amd64-m64-gcc42-nn", "./soplex_base.amd64-m64-gcc42-nn",
                           "./gcc_base.amd64-m64-gcc42-nn", "./dealII_base.amd64-m64-gcc42-nn",
                           "./povray_base.amd64-m64-gcc42-nn", "./omnetpp_base.amd64-m64-gcc42-nn",
                           "./Xalan_base.amd64-m64-gcc42-nn", "./hmmer_base.amd64-m64-gcc42-nn",
    "./reporter"
    };

const char *path_table[] = {"/home/cuiran/spec2017/benchspec/CPU/508.namd_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/510.parest_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/511.povray_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/519.lbm_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/526.blender_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/538.imagick_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/544.nab_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/619.lbm_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/638.imagick_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/644.nab_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/500.perlbench_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/502.gcc_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/505.mcf_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/520.omnetpp_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/523.xalancbmk_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/525.x264_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/531.deepsjeng_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/541.leela_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/557.xz_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/600.perlbench_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/602.gcc_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/605.mcf_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/620.omnetpp_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/623.xalancbmk_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/625.x264_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/631.deepsjeng_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/641.leela_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/657.xz_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/503.bwaves_r/run/run_base_refrate_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/603.bwaves_s/run/run_base_refspeed_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/548.exchange2_r/run/run_base_refrate_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/648.exchange2_s/run/run_base_refspeed_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/507.cactuBSSN_r/run/run_base_refrate_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/607.cactuBSSN_s/run/run_base_refspeed_mytest-m64.0001",
                            "/home/cuiran/spec2017/benchspec/CPU/521.wrf_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/621.wrf_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/527.cam4_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/627.cam4_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/549.fotonik3d_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/649.fotonik3d_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/554.roms_r/run/run_base_refrate_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/654.roms_s/run/run_base_refspeed_mytest-m64.0000",
                            "/home/cuiran/spec2017/benchspec/CPU/628.pop2_s/run/run_base_refspeed_mytest-m64.0000",
                            //spec2006
                            "/home/cuiran/spec2006/benchspec/CPU2006/435.gromacs/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/401.bzip2/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/429.mcf/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/454.calculix/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/482.sphinx3/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/470.lbm/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/462.libquantum/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/450.soplex/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/403.gcc/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/447.dealII/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/453.povray/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/471.omnetpp/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/483.xalancbmk/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/spec2006/benchspec/CPU2006/456.hmmer/run/run_base_ref_amd64-m64-gcc42-nn.0000/",
                            "/home/cuiran/reporter"};
const char *benchmark_name[] = {"508", "510", "511", "519", "526", "538", "544", "619", "638", "644",
                                "500", "502", "505", "520", "523", "525", "531", "541", "557", "600",
                                "602", "605", "620", "623", "625", "631", "641", "657", /**/ "503", "603",
                                "548", "648", "507", "607", "521", "621", "527", "627", "549", "649", "554",
                                "654", "628",
                                "gromacs"/*43*/, "bzip", "mcf", "calculix", "sphinx3", "lbm", 
                                "libquantum"/*49*/, "soplex", "gcc", "dealII", "povray", "omnetpp", "xalancbmk", "hmmer","reporter"};