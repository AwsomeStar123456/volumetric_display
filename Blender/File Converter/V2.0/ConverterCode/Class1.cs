using System;
using System.Drawing;
using System.IO;
using System.Linq;

namespace ConverterCode
{
    public class MyImageConverter
    {
        public void DirectoryImagesToTextFile(string inputPath, string outputPath)
        {
            using (StreamWriter writer = new StreamWriter(outputPath))
            {
                string[] imagePaths = Directory.GetFiles(inputPath, "*.png");

                foreach (string imagePath in imagePaths)
                {
                    ImageToTextFile(imagePath, writer);
                }
            }
        }   

        public void ImageToTextFile(string inputPath, StreamWriter writer)
        {
            using (Bitmap image = new Bitmap(inputPath))

                for (int x = 0; x < image.Width; x++)
                {
                    string binaryColumn = "";
                    for (int y = 0; y < image.Height; y++)
                    {
                        Color pixelColor = image.GetPixel(x, y);
                        // Assuming white is represented as 255,255,255 and black as 0,0,0
                        // You might need to adjust the threshold based on your image
                        binaryColumn += pixelColor.GetBrightness() >= 0.5 ? "1" : "0";
                    }
                    writer.WriteLine(binaryColumn);
                }



        }



    }
}