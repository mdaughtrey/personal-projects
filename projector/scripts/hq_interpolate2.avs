#PARAMETERS
#----------------------------------------------------------------------------------------------------------------------------
trim_begin=2  trim_end=10  play_speed=18   #trim frames and play speed (PAL: 16.6666 or 18.75)
numerator= 25  #numerator for the interpolator (final frame rate)
denumerator= 1 #denumerator  example: 60000/1001= 59.94fps
#COLOR AND LEVELS PARAMATERS
#----------------------------------------------------------------------------------------------------------------------------
saturation=1.2   #for all outputs
gamma= 1.2 # for all outputs 
blue= 0  red= 0  #manual color adjustment, when returning result3 or result4. Values can be positive or negative
black_level=0  white_level=255 output_black=0  output_white=255 # manual levels, when returning result4
#AUTO LEVELS PARAMETERS
#--------------------------------------------------------------------------------------------------------------------------------
autolev_low= 6     # limit of autolevels low output
autolev_high= 235  # limit of autolevels high output
#SIZE, CROP AND BORDERS PARAMETERS
#----------------------------------------------------------------------------------------------------------------------------
CLeft=32  CTop=32  CRight=32  CBottom=32  #crop values after Depan and before final resizing 
#W=720  H=576  #final size after cropping 
W=1920  H=1080  #final size after cropping 
bord_left=0  bord_top=0  bord_right=0  bord_bot=0  #720p= borders 150
#STABILISING PARAMETERS, YOU REALY MUST USE RESULTS7 TO CHECK STABILISATION!
#----------------------------------------------------------------------------------------------------------------------------
maxstabH=20 
maxstabV=20 #maximum values for the stabiliser (in pixels) 20 is a good start value
est_left=20   est_top=60  est_right=60  est_bottom=60  #crop values for special Estimate clip
trust_value= 1.0     # scene change detection, higher= more sensitive
cutoff_value= 0.5   # no need to change this, but you can play with it and see what you get
#CLEANING PARAMETERS
#--------------------------------------------------------------------------------------------------------------
dirt_strenght=30 # set this lower for clean films.
#DENOISING PARAMETERS
#----------------------------------------------------------------------------------------------------------------------------
denoising_strenght= 300  #denoising level of second denoiser: MVDegrainMulti() 
denoising_frames= 3  #number of frames for averaging (forwards and backwards) 3 is a good start value
block_size= 16  #block size of MVDegrainMulti()
block_size_v= 16
block_over= 8  #block overlapping of MVDegrainMulti()
# SHARPENING PARAMETERS
#--------------------------------------------------------------------------------------------------------------------------------
USM_sharp_ness= 40   USM_radi_us=3  #this is the start value for the unsharpmask sharpening
                                    #do not set radius less then 3 
                                    #the script will automatically add two other steps with lower radius 
last_sharp= 0.1 #final sharpening step after interpolation
last_blur= 0.2 #this smooths out the heavy sharpening effects
# END VARIABLES, BEGIN SCRIPT
#=================================================================================================================================
SetMemoryMax(2000)  #set this to 1/3 of the available memory
LoadPlugin("plugins/Deflicker.dll")
Loadplugin("plugins/Depan.dll")
LoadPlugin("plugins/DepanEstimate.dll")
Loadplugin("plugins/removegrain.dll")
LoadPlugin("plugins/removedirt.dll")
LoadPlugin("plugins/MVTools.dll")
LoadPlugin("plugins/MVTools2.dll")
Loadplugin("plugins/warpsharp.dll")
LoadPlugin("plugins/autolevels_06.dll")
Import("c:\\windows\\system\\plugins/03_RemoveDirtMC.avs")
source= AviSource(film).assumefps(play_speed).trim(trim_begin,0).converttoYV12()
trimming= framecount(source)-trim_end
source1= trim(source,0,trimming)
#STABILIZING/CROPPING
#...........................................................................................................................................
stab_reference= source1.crop(20,20,-20,-20).colorYUV(autogain=true).crop(est_left,est_top,-est_right,-est_bottom)
mdata=DePanEstimate(stab_reference,trust=trust_value,dxmax=maxstabH,dymax=maxstabV)
stab=DePanStabilize(source1,data=mdata,cutoff=cutoff_value,dxmax=maxstabH,dymax=maxstabV,method=0,mirror=15).deflicker()
stab2= stab.crop(CLeft,CTop,-CRight,-CBottom)
stab3=DePanStabilize(source1,data=mdata,cutoff=cutoff_value,dxmax=maxstabH,dymax=maxstabV,method=0,info=true)
WS= width(stab)
HS= height(stab)
stab4= stab3.addborders(10,10,10,10,$B1B1B1).Lanczos4Resize(WS,HS)
stab5= Lanczos4Resize(stab2,W,H).sharpen(0.5)
#UNSHARPMASK AUTO_PARAMETERS
#-------------------------------------------------------------------------------------------------------------------------------------------
USM_sharp_ness1 = USM_sharp_ness
USM_sharp_ness2 = USM_sharp_ness+(USM_sharp_ness/2)
USM_sharp_ness3 = USM_sharp_ness*2
USM_radi_us1 = USM_radi_us
USM_radi_us2 = USM_radi_us-1
USM_radi_us3 = USM_radi_us2-1
#CLEANING/PRESHARPENING/RESIZING
#..........................................................................................................................................
noise_baseclip= stab2.levels(0,gamma,255,0,255).tweak(sat=saturation)
cleaned= RemoveDirtMC(noise_baseclip,dirt_strenght).unsharpmask(USM_sharp_ness1,USM_radi_us1,0)\
.unsharpmask(USM_sharp_ness2,USM_radi_us2,0).Lanczos4Resize(W,H)
#DEGRAINING/SHARPENING
#...................................................................................................................................................................
vectors= cleaned.MVAnalyseMulti(refframes=denoising_frames, pel=2, blksize=block_size, blksizev= block_size_v, overlap=block_over, idx=1)
denoised= cleaned.MVDegrainMulti(vectors, thSAD=denoising_strenght, SadMode=1, idx=2).unsharpmask(USM_sharp_ness3,USM_radi_us3,0)
#CHANGING FRAME RATE WITH INTERPOLATION/FINALSHARPENING
#............................................................................................................................................................
super= denoised.MSuper()
backward_vec= MAnalyse(super, blksize=block_size, blksizev= block_size_v, overlap=block_over, isb=true)
forward_vec= MAnalyse(super,blksize=block_size, blksizev= block_size_v, overlap=block_over, isb= false)
interpolated= denoised.MFlowFps(super, backward_vec, forward_vec, num=numerator, den= denumerator, ml=100)\
.sharpen(last_sharp,mmx=false).sharpen(last_sharp,mmx=false).blur(last_blur,mmx=false)

#RESULT1: AUTOLEVELS,AUTOWHITE
#......................................................................................................................................................................
result1= interpolated.converttoRGB24().autolevels(output_low= autolev_low, output_high= autolev_high)\
.converttoYV12().coloryuv(autowhite=true).addborders(bord_left, bord_top, bord_right, bord_bot)
#RESULT2: MANUAL LEVELS, AUTOWHITE
#......................................................................................................................................................................
result2= interpolated.levels(black_level,1.0,white_level,0,255).coloryuv(autowhite=true)\
.addborders(bord_left, bord_top, bord_right, bord_bot)
#RESULT3: AUTOLEVELS, MANUAL COLOR CORRECTIONS
#.....................................................................................................................................................................
result3= interpolated.coloryuv(off_U=blue,off_V=red).converttoRGB24().autolevels(output_low= autolev_low, output_high= autolev_high)\
.converttoYV12().addborders(bord_left, bord_top, bord_right, bord_bot)

#RESULT4: MANUAL LEVELS, MANUAL COLOR CORRECTIONS
#.....................................................................................................................................................................
result4= interpolated.coloryuv(off_U=blue,off_V=red).levels(black_level,1.0,white_level,0,255)\
.addborders(bord_left, bord_top, bord_right, bord_bot)

#RESULT5: SPECIAL SERVICE CLIP FOR RESULT S5
#.....................................................................................................................................................................
result5= overlay(source1,greyscale(stab_reference),x=est_left,y=est_top).addborders(2,2,2,2,$FFFFFF).Lanczos4Resize(WS,HS)
#PARAMETERS FOR THE COMPARISONS
#.....................................................................................................................................................................
W2= W+bord_left+bord_right
H2= H+bord_top+bord_bot
final_framerate= numerator/denumerator
source4=Lanczos4Resize(source1,W2,H2).changeFPS(final_framerate)
#COMPARISONS: ORIGINAL VS RESULTS
#......................................................................................................................................................................
resultS1= stackhorizontal(subtitle(source4,"original",size=28,align=2),subtitle(result1,"result1: autolevels, autowhite",size=28,align=2))
resultS2= stackhorizontal(subtitle(source4,"original",size=28,align=2),subtitle(result2,"result2: autowhite, manual levels correction",size=28,align=2))
resultS3= stackhorizontal(subtitle(source4,"original",size=28,align=2),subtitle(result3,"result3: autolevels, manual color correction",size=28,align=2))
resultS4= stackhorizontal(subtitle(source4,"original",size=28,align=2),subtitle(result4,"result4: manual colors and levels correction",size=28,align=2))
resultS5= stackhorizontal(subtitle(result3,"result3: auto levels, manual color correction",size=28,align=2),subtitle(result4,"result4: manual colors and levels correction",size=28,align=2))
resultS6= stackhorizontal(subtitle(result1,"result1: autolevels, autowhite",size=28,align=2),subtitle(result2,"result2: manual levels, autowhite",size=28,align=2))

#SPECIAL COMPARISON CLIP FOR TESTING THE STABILIZER
#.........................................................................................................................................................................
resultS7= stackhorizontal(subtitle(result5,"baseclip for stabiliser -only the B/W clip is used",size=32,align=2),\
subtitle(stab4,"test stabiliser: dx=horizontal, dy=vertical",size=32,align=5)).converttoYUY2()
Eval(result)#.converttoRGB24()
