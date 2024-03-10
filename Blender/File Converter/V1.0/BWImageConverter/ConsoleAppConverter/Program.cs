using System;
using Converter;

namespace ConsoleAppConverter
{
    internal class Program
    {
        static void Main(string[] args)
        {
            ImageReader imageReader = new ImageReader();
            imageReader.ConvertImagesInDirectory("C:/tmp", "E:/!!IRL/School/!!University/UofU/ECE3992/Project/ConvertedImages");
            imageReader.GenerateHeaderFile("E:/!!IRL/School/!!University/UofU/ECE3992/Project/ConvertedImages", "C:/Users/Semra/Documents/Pico-v1.5.1/pico-ledarray2/imagedata.h");
        }
    }
}