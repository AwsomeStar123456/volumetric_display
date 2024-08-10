using ConverterCode;
using System;
using System.Drawing;
using System.IO;
using System.Linq;

// See https://aka.ms/new-console-template for more information
Console.WriteLine("Hello, World!");

MyImageConverter converter = new MyImageConverter();
converter.DirectoryImagesToTextFile("../../../../../../!Volumetric Blender Data/Straight Cube/Render Data", "../../../../../../!Volumetric Blender Data/Straight Cube/Text Data/Output.txt");
converter.GenerateHeaderFile("../../../../../../!Volumetric Blender Data/Straight Cube/Text Data/Output.txt", "../../../../../../!Volumetric Blender Data/Straight Cube/Header Data");