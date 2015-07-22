import re, os

def get_dirs(path):
	if os.path.exists(path):
		return os.listdir(path)
	else:
		return -1

def rename():
    path = raw_input('Enter the path where there are the files you want to rename---->:')
    files = get_dirs(path)
    print files

    name_modify = raw_input('Enter the prefix name that you add---->:')

    goal_ext ="."+raw_input("Enter the type of files which you want to rename(E.g.jpg,png)---->:")
    print goal_ext

    if len(files) !=0:
    	filename_list = ''
        changeCount = 0
    	for f in files:
    		filename_list += f +';'
    		ext = os.path.splitext(f)[1]
    		if (ext == goal_ext):
                    changeCount+=1
                    newname= name_modify+'_%d' %changeCount
                    newname2 = newname + ext
                    oldpath = path + '/' + f
                    newpath = path + '/' + newname2
                    os.rename(oldpath, newpath)
                else:
                    continue

    	filename_list = '[' + filename_list + ']'

    	print filename_list

if __name__ == '__main__':

    rename()

    #print(''.join(x[0] for x in list))

