using ConverterCode;
using System;
using System.Drawing;
using System.IO;
using System.Linq;

string baseDirectory = "../../../../../../!Volumetric Blender Data/Water";
Console.WriteLine("Initiating image conversion.");

MyImageConverter converter = new MyImageConverter();
converter.DirectoryImagesToTextFile(baseDirectory + "/Render Data", baseDirectory + "/Text Data/Output.txt");
converter.GenerateHeaderFile(baseDirectory + "/Text Data/Output.txt", baseDirectory + "/Header Data");

Console.WriteLine("Image conversion complete.");
Console.WriteLine("Completed Path: " + baseDirectory);