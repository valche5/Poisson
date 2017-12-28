# Poisson
Projet TSMA

usage : /home/valentin/Dropbox/Fac/TIA/Poisson/cmake-build-release/final [-m] image_directory result_name
	-m :              specify to use mixing gradients
	image_directory : path to the directory which contains images. image_directory must contain 3 image files : source.png, target.png and mask.pgm.
	                  source.png is the image to be cloned in target.png, mask.pgm is the portion of source.png that is cloned in target.png
	result_name :     the filename of the resulting image to write in image_directory
