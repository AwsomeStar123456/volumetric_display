namespace OptimizationCode
{
    using System;
    using System.IO;
    using System.Linq;

    public class ImageComparer
    {
        public void CompareFiles(string directoryPath, string outputDirectory)
        {
            var files = Directory.GetFiles(directoryPath, "*.txt").OrderBy(f => f).ToArray();
            for (int i = 0; i < files.Length - 1; i++)
            {
                CompareAndWriteChanges(files[i], files[i + 1], outputDirectory);
            }

            // Compare the last file with the first one
            CompareAndWriteChanges(files[^1], files[0], outputDirectory);
        }

        private void CompareAndWriteChanges(string file1, string file2, string outputDirectory)
        {
            var array1 = FileTo2DArray(file1);
            var array2 = FileTo2DArray(file2);

            using (var writer = new StreamWriter(Path.Combine(outputDirectory, $"{Path.GetFileNameWithoutExtension(file1)}{Path.GetFileNameWithoutExtension(file2)}.txt")))
            {
                for (int x = 0; x < array1.GetLength(0); x++)
                {
                    for (int y = 0; y < array1.GetLength(1); y++)
                    {
                        if (array1[x, y] != array2[x, y])
                        {
                            writer.WriteLine($"{x},{y},{array2[x, y]}");
                        }
                    }
                }
            }
        }

        private int[,] FileTo2DArray(string filePath)
        {
            var lines = File.ReadAllLines(filePath);
            var dimensions = lines[0].Split(',').Select(int.Parse).ToArray();
            var array = new int[dimensions[1], dimensions[0]];

            for (int i = 1; i < lines.Length; i++)
            {
                var row = lines[i].Split(',').Select(int.Parse).ToArray();
                for (int j = 0; j < row.Length; j++)
                {
                    array[i - 1, j] = row[j];
                }
            }

            return array;
        }
    }
}