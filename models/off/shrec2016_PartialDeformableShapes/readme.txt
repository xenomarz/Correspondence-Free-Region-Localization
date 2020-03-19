This archive contains the training shapes for the contest "SHREC 2016 - Partial Matching Of Deformable Shapes".
Files are subdivided in three folders:
- cuts: contains shapes and ground-truth files of the cuts dataset. 
- holes: contains shapes and ground-truth files of the holes dataset. 
- null: shapes in a canonical pose to be used for matching.
Shapes are saved in off format. 
Ground-truth is provided for each deformed shape as a ASCII file with baryc_gt (baryc_gt_sym for the symmetric ground-truth) extension. Each row contains the triangle index and the barycentric weight of the corresponding point of the relative shape in canonical pose.
Shapes are based on the TOSCA high-resolution dataset and full shapes have approximately 10000 vertices each.
for more information about the datasets and the contest visit http://www.dais.unive.it/~shrec2016/.


Copyright (c) Cosmo Luca