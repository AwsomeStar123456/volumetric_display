using System;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;

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

        public void GenerateHeaderFile(string inputPath, string outputPath)
        {
            if (!File.Exists(inputPath))
            {
                Console.WriteLine("File does not exist: " + inputPath);
                return;
            }

            var lines = File.ReadAllLines(inputPath);
            if (lines.Length == 0)
            {
                Console.WriteLine("File is empty.");
                return;
            }

            int frames = (lines.Length  / 24) / 9;
            Console.WriteLine($"Total lines: {lines.Length}, Frames: {frames}");

            if (frames == 0)
            {
                Console.WriteLine("No frames to process. Check the total number of lines in your file.");
                return;
            }

            ushort[][][] imageData = new ushort[frames][][];

            for (int i = 0; i < frames; i++)
            {
                imageData[i] = new ushort[24][];
                for (int j = 0; j < 24; j++)
                {
                    imageData[i][j] = new ushort[9];
                    for (int k = 0; k < 9; k++)
                    {
                        int lineIndex = i * 9 * 24 + j * 9 + k;
                        if (lineIndex < lines.Length)
                        {
                            imageData[i][j][k] = Convert.ToUInt16(lines[lineIndex], 2);
                        }
                    }
                }
            }

            StringBuilder headerContent = new StringBuilder();
            headerContent.AppendLine("uint16_t image_data[" + frames + "][24][9] = {");
            for (int i = 0; i < frames; i++)
            {
                headerContent.AppendLine("{");
                for (int j = 0; j < 24; j++)
                {
                    headerContent.Append("{");
                    for (int k = 0; k < 9; k++)
                    {
                        // Convert the ushort value back to a binary string with '0b' prefix
                        string binaryString = "0b" + Convert.ToString(imageData[i][j][k], 2).PadLeft(16, '0');
                        headerContent.Append(binaryString);
                        if (k < 8) headerContent.Append(", ");
                    }
                    headerContent.Append("}");
                    if (j < 23) headerContent.AppendLine(",");
                }
                headerContent.AppendLine("},");
            }
            headerContent.AppendLine("};");

            File.WriteAllText(outputPath + "/ImageData.h", headerContent.ToString());
        }
    }
}