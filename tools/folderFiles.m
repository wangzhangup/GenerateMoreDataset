function [outFiles]=folderFiles(inFolder,varargin)
% FOLDERFILES Reads filename contents of a folder.
% [outFiles]=folderFiles(inFolder,fileNames) Reads a group of (fileNames) 
% from a folder specified by (inFolder), and generates the output in the 
% form of a char array. 
% fileNames is the criteria that specifies the files, e.g., *.jpg. If
% fileNames is left as a blank (default) this function reads all contents, 
% but this is not preferred since some folders containing images will also 
% have system files such as (Thumbs.db) or (folder.ini) beside subfolders. 
% 
% [outFiles]=folderFiles(inFolder) Reads all contents from the folder 
% name(inFolder) and returns the names in (outFiles) with type char.
% Output here will include all the contents of the folder, i.e., files and
% subfolders(to the first tree level).
% 
% Examples:
% ---------
% 1) To search for files of the extension *.jpg
%    G=folderFiles('M:\MyFolder\','*.jpg')
% 
% 2) To search for all files (*.*)
%    G=folderFiles('M:\MyFolder\')      or 
%    G=folderFiles('M:\MyFolder\','*.*')
% 
% 3) To search for files that start with the letter r (r*)
%    G=folderFiles('M:\MyFolder\','r*')
% 
% 4) To search for a file with the letter (r) anywhere in the filename(*r*)
%    G=folderFiles('M:\MyFolder\','*r*')
% 
% -------------------------------------------
% You can also open files individually from built-in M-function as:
% [filename, pathname, filterindex] = uigetfile( ...
% {  '*.jpg','MAT-files (*.jpj)'; ...
%  '*.mdl','Models (*.mdl)'; ...
%  '*.*',  'All Files (*.*)'}, ...
% 
%   See also PATH, ADDPATH, UIGETFILE

% folderFiles.m Function
% by Ahmed A. Selman, 17 March 2013
% Revised, 23 March 2013
a = nargin;
if a > 2
    error('myApp:argChk', 'Too many input arguments.')
end
if a == 1
    y = '*.*';
else
    N = varargin;
    y = N{1};
end
if( ~ischar(inFolder) || ~ischar(y) );
    error('MATLAB:catdirs:ArgNotString', 'All arguments must be strings.');
end
addpath (inFolder); 
fileNames = fullfile(inFolder);
dirOutput = dir(fullfile(fileNames,y));
out = {dirOutput.name}'; 
outFiles = char(out);
rmpath (inFolder);
end 