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
#i=1000 #-iteration n/a
#p=1 #-subpixel -p

echo "run_hedcuter1"
./run_hedcuter -I images/einstein-medium.png -O results/hedcuter/1-1.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein-medium.png -O results/hedcuter/1-2.svg -black -avg -n $n -radius $r -maxD $d

echo "run_voroni1"
./run_voronoi -I images/einstein-medium.png -O results/voronoi/1-1.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein-medium.png -O results/voronoi/1-2.svg -s $n -z $z -t $t

n=500
echo "run_hedcuter2"
./run_hedcuter -I images/einstein-medium.png -O results/hedcuter/2-1.svg -black -avg -n $n -radius $r -maxD $d

n=2000
./run_hedcuter -I images/einstein-medium.png -O results/hedcuter/2-2.svg -black -avg -n $n -radius $r -maxD $d

n=500
echo "run_voroni2"
./run_voronoi -I images/einstein-medium.png -O results/voronoi/2-1.svg -s $n -z $z -t $t

n=2000
./run_voronoi -I images/einstein-medium.png -O results/voronoi/2-2.svg -s $n -z $z -t $t

n=1000
echo "run_hedcuter3"
./run_hedcuter -I images/einstein-medium-high-contrast.png -O results/hedcuter/3-1.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein-medium-low-contrast.png -O results/hedcuter/3-2.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein-medium-high-bright.png -O results/hedcuter/3-3.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein-medium-low-bright.png -O results/hedcuter/3-4.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein.png -O results/hedcuter/3-5.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/einstein-small.png -O results/hedcuter/3-6.svg -black -avg -n $n -radius $r -maxD $d

echo "run_voroni3"
./run_voronoi -I images/einstein-medium-high-contrast.png -O results/voronoi/3-1.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein-medium-low-contrast.png -O results/voronoi/3-2.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein-medium-high-bright.png -O results/voronoi/3-3.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein-medium-low-bright.png -O results/voronoi/3-4.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein.png -O results/voronoi/3-5.svg -s $n -z $z -t $t
./run_voronoi -I images/einstein-small.png -O results/voronoi/3-6.svg -s $n -z $z -t $t


echo "run_hedcuter4"
./run_hedcuter -I images/phoenix.png -O results/hedcuter/4-1.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/gradient.png -O results/hedcuter/4-2.svg -black -avg -n $n -radius $r -maxD $d
./run_hedcuter -I images/erinking.png -O results/hedcuter/4-3.svg -black -avg -n $n -radius $r -maxD $d

echo "run_voroni4"
./run_voronoi -I images/phoenix.png -O results/voronoi/4-1.svg -s $n -z $z -t $t
./run_voronoi -I images/gradient.png -O results/voronoi/4-2.svg -s $n -z $z -t $t
./run_voronoi -I images/erinking.png -O results/voronoi/4-3.svg -s $n -z $z -t $t

for pro_file in `ls results/voronoi/*.svg`
do

app_file=$(basename "$pro_file")
app_file="${app_file%.*}"

svg2pdf $pro_file results/voronoi/$app_file.pdf

done

for pro_file in `ls results/hedcuter/*.svg`
do

app_file=$(basename "$pro_file")
app_file="${app_file%.*}"

svg2pdf $pro_file results/hedcuter/$app_file.pdf

done
