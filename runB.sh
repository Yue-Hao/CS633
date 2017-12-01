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
t=0.14 #
r=7 #-radius -z
z=0.7
i=1000 #-iteration n/a
#p=1 #-subpixel -p

echo "run_hedcuter1"
./run_hedcuter -I images/obama_small.png -O results/hedcuter/B-1.svg -black -avg -n $n -radius $r -maxD $d

./run_hedcuter -I images/obama_small.png -O results/hedcuter/B-2.svg -black -avg -area_radius -n $n -radius $r -maxD $d

for pro_file in `ls results/hedcuter/*.svg`
do

app_file=$(basename "$pro_file")
app_file="${app_file%.*}"

svg2pdf $pro_file results/hedcuter/$app_file.pdf

done
