#!/bin/bash

#"input-file,I"
#"output-file,O"
#"-debug" false
#"-n"  sample_size = 1000
# "-uniform_radius" hedcut.uniform_disk_size = false;
#"-radius"  hedcut.disk_size = 1;
#"-iteration" hedcut.cvt_iteration_limit = 100;
#"-maxD" hedcut.max_site_displacement 1.01
#"-black" hedcut.black_disk = false;
#"-avg" hedcut.average_termination = false;
#"-gpu" hedcut.gpu = false;
#"-subpixel" hedcut.subpixels = 1;


#"input-file,I"
#"output-file,O"
#"stipples,s" 4000 "Number of Stipple Points to use" )
#"colour-output,c", "Produce a coloured stipple drawing" );
#"threshold,t", 0.1f "How long to wait for Voronoi diagram to converge"
#"no-overlap,n", "Ensure that stipple points do not overlap with each other" )
#"fixed-radius,f", "Fixed radius stipple points imply a significant loss of tonal properties" )
#"sizing-factor,z", 1.0f "The final stipple radius is multiplied by this factor" )
#"subpixels,p" 5, "Controls the tile size of centroid computations." )
#"log,l", "Determines output verbosity" );

n=1000 #-n -s
d=1.0 #-maxD -t
t=0.5 #
r=7 #-radius -z
z=0.7
#i=1000 #-iteration n/a
#p=1 #-subpixel -p

dstart=200
dend=2000
m=200

rm results/hedcuter.txt
rm results/voronoi.txt

for ((i=$dstart; i<=$dend; i=$i+$m))
do
#echo $i
./run_hedcuter -I images/einstein-medium.png -O temp.svg -black -avg -n $i -radius $r -maxD $d | grep 'time' >> results/hedcuter.txt
done

for ((i=$dstart; i<=$dend; i=$i+$m))
do
#echo $i
./run_voronoi -I images/einstein-medium.png -O temp.svg -s $i -z $z -t $t | grep 'Completed' >> results/voronoi.txt
done


