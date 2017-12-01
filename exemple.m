%mex patchmatch.cpp

a = imread('Nola4.jpg');
b = imread('Nola3.jpg');

tic
patchSize = 3;
nIter = 5;
randomSearch = true;
[c, nnf, nnfd] = patchmatch(a, b, patchSize, nIter, randomSearch);
toc

subplot(2, 2, 1);
imshow(a);
title('a');
subplot(2, 2, 2);
imshow(b);
title('b');
subplot(2, 2, 3);
imshow(c);
title('a rec');
subplot(2, 2, 4);
colormap parula
imagesc(double(nnfd(:,:,1) + 1));
colorbar
title('nnfd');
axis image

imwrite(c, ['a_', num2str(patchSize), '_', num2str(nIter), '.png'])