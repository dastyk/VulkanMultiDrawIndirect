start /wait /b C:\VulkanSDK\1.0.46.0\Bin\glslangValidator.exe -V -S vert -o vertex.spv vertex.glsl
start /wait /b C:\VulkanSDK\1.0.46.0\Bin\glslangValidator.exe -V -S frag -o fragment.spv fragment.glsl
start /wait /b C:\VulkanSDK\1.0.46.0\Bin\glslangValidator.exe -V -S comp -o compute.spv compute.glsl
pause