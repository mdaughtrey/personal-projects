#!/bin/bash
NAME=avxsynth
FRAMES=${PWD}/share
SCRIPT=/tmp/avx.avs
OUT='/frames/avxout/%08d.png'
let NUMFRAMES=$(ls ${FRAMES}/????????.png | wc -l)

(cat <<AVS
LoadPlugin("/AviSynthPlus/avisynth-build/plugins/ImageSeq/libimageseq.so")
LoadPlugin("/RemoveDirt/build/RemoveDirt/libremovedirt.so")
LoadPlugin("/mvtools/build/Sources/libmvtools2.so")
LoadPlugin("/RgTools/build/RgTools/librgtools.so")
LoadPlugin("/mvtools/build/DePanEstimate/libdepanestimate.so")
LoadPlugin("/mvtools/build/DePan/libdepan.so")
LoadPlugin("/mvtools/build/Sources/libmvtools2.so")
function RemoveDirt(clip input, bool "_grey", int "repmode") 
{
    _grey=default(_grey, false)
	repmode=default(repmode, 16)
	clmode=17
	clensed=Clense(input, grey=_grey, cache=4)
	sbegin = ForwardClense(input, grey=_grey, cache=-1)
	send = BackwardClense(input, grey=_grey, cache=-1)
	alt=Repair(SCSelect(input, sbegin, send, clensed, debug=true), input, mode=repmode, modeU = _grey ? -1 : repmode ) 
	restore=Repair(clensed, input, mode=repmode, modeU = _grey ? -1 : repmode)
	corrected=RestoreMotionBlocks(clensed, restore, neighbour=input, alternative=alt, gmthreshold=70, dist=1, dmode=2, debug=false, noise=10, noisy=12, grey=_grey)
	return RemoveGrain(corrected, mode=clmode, modeU = _grey ? -1 : clmode )
}	
RemoveDirt(ImageReader("/frames/%08d.png", fps=18.0, end=${NUMFRAMES}).converttoYV12())
AVS
)  > $(basename ${SCRIPT})

docker run --rm -h ${NAME} --name ${NAME} -t -v ${FRAMES}:/frames --entrypoint /ffmpeg/ffmpeg  ${NAME}:latest  -i ${SCRIPT} -y ${OUT}
