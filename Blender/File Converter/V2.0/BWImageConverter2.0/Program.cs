using ConverterCode;
using System;
using System.Drawing;
using System.IO;
using System.Linq;

// See https://aka.ms/new-console-template for more information
Console.WriteLine("Hello, World!");

MyImageConverter converter = new MyImageConverter();
converter.DirectoryImagesToTextFile("../../../../../../!Volumetric Blender Data/Cube/Render Data", "../../../../../../!Volumetric Blender Data/Cube/Text Data/Output.txt");
