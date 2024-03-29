# Shader processor

import glob
import os

def main():
	srcPath = './Shaders/src/'
	binPath = './Shaders/bin/'
	logPath = './Shaders/log/'

	shaderFormats = [
		'*.vert', 
		'*.tesc',
		'*.tese',
		'*.geom',
		'*.frag',
		'*.comp']
	if not os.path.isdir(binPath):
		os.mkdir(binPath) 
	
	for format in shaderFormats:
		for shaderFile in glob.glob(srcPath + format):
			shaderName = os.path.basename(shaderFile)
			os.system("glslangValidator.exe -V -H {0} -o {1} > {2}".format(
				srcPath + shaderName,
				binPath + shaderName + '.spv',
				logPath + shaderName))
	os.system("pause")

if __name__ == "__main__":
	main()