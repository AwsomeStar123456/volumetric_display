using System;
using System.Drawing;
using System.IO;
using System.Linq;

namespace Converter
{
    public class ImageReader
    {
        public void ConvertImagesInDirectory(string inputDirectory, string outputDirectory)
        {
            var imageFiles = Directory.EnumerateFiles(inputDirectory, "*.png");

            foreach (var imagePath in imageFiles)
            {
                string outputFilePath = Path.Combine(outputDirectory, Path.GetFileNameWithoutExtension(imagePath) + ".txt");
                ConvertImageToBinaryArray(imagePath, outputFilePath);
            }
        }

        private void ConvertImageToBinaryArray(string imagePath, string outputPath)
        {
            using (Bitmap bitmap = new Bitmap(imagePath))
            {
                int width = bitmap.Width;
                int height = bitmap.Height;

                int[,] array = new int[height, width];

                for (int i = 0; i < height; i++)
                {
                    for (int j = 0; j < width; j++)
                    {
                        Color pixel = bitmap.GetPixel(j, i);

                        // Assuming the image is grayscale, we only need to check one color channel
                        if (pixel.R > 128)
                        {
                            array[i, j] = 1; // White pixel
                        }
                        else
                        {
                            array[i, j] = 0; // Black pixel
                        }
                    }
                }

                // Write the array to a text file
                using (StreamWriter writer = new StreamWriter(outputPath))
                {
                    writer.WriteLine($"{width}, {height}");

                    for (int i = 0; i < height; i++)
                    {
                        for (int j = 0; j < width; j++)
                        {
                            writer.Write(array[i, j]);
                            if (j < width - 1)
                            {
                                writer.Write(", ");
                            }
                        }
                        writer.WriteLine();
                    }
                }
            }
        }

        public void GenerateHeaderFile(string inputDirectory, string outputFilePath)
        {
            var textFiles = Directory.EnumerateFiles(inputDirectory, "*.txt");

            using (StreamWriter writer = new StreamWriter(outputFilePath))
            {
                foreach (var textFilePath in textFiles)
                {
                    string arrayName = Path.GetFileNameWithoutExtension(textFilePath);
                    string[] lines = File.ReadAllLines(textFilePath);

                    writer.WriteLine($"int {arrayName}[] = {{");

                    foreach (var line in lines)
                    {
                        writer.WriteLine("    " + line.Replace(", ", ",") + ",");
                    }

                    writer.WriteLine("};");
                    writer.WriteLine();
                }
            }
        }
    }
}