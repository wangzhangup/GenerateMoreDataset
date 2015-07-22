function rotationImage(folder,filetype)

	filelist = folderFiles(folder,filetype);
	filelist2 = cellstr(filelist);
	[x, y] = size(filelist2);


	cd(folder);
	for i = 1:x
		filename = filelist2(i);
		%fn_regexp = regexp(filename, 'scene_[0-9]+','match');
		filename = filename{1};
		% if isempty(fn_regexp{1})
		% 	continue;
		% else
		% 	degree = fn_regexp{1}{1}(7:8);
		% 	image = imread(filename);
		% 	degree = str2num(degree);
		% 	if degree == 30
		% 		image_rotation = imrotate(image, -14);
		% 		filename = ['rotate_',filename];
		% 	end
		% 	if degree == 45
		% 		image_rotation = imrotate(image, -19);
		% 		filename = ['rotate_',filename];
		% 	end
		% 	if degree == 60
		% 		image_rotation = imrotate(image, -20);
		% 		filename = ['rotate_',filename];
		% 	end
		% 	if degree == 75
		% 		image_rotation = imrotate(image, -20);
		% 		filename = ['rotate_',filename];
		% 	end

		% 	imwrite(image_rotation, filename);
		% end
		image = imread(filename);
		filename = ['mirror_',filename];
		image_mirror = flipdim(image,2);	
		imwrite(image_mirror, filename);
	end

	cd('..');


end