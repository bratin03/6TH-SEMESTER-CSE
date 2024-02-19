#include <iostream>
#include <fstream>
#include <string>
#include <cerrno>
#include <cstring>

void compareFiles(const std::string &file1Path, const std::string &file2Path)
{
    // Open the first file
    std::ifstream file1(file1Path);
    if (!file1.is_open())
    {
        // Handle error if unable to open the first file
        std::cerr << "Error: Unable to open File 1 (" << file1Path << ")." << std::endl;
        return;
    }

    // Open the second file
    std::ifstream file2(file2Path);
    if (!file2.is_open())
    {
        // Handle error if unable to open the second file
        std::cerr << "Error: Unable to open File 2 (" << file2Path << ")." << std::endl;
        file1.close(); // Close the first file before returning
        return;
    }

    char char1, char2;
    int charNumber = 1;

    // Compare files character by character
    while (file1.get(char1) && file2.get(char2))
    {
        // Print the characters being compared
        std::cout << char1 << " " << char2 << std::endl;

        // Check for a difference
        if (char1 != char2)
        {
            std::cout << "Difference found at character " << charNumber << ":" << std::endl;
            std::cout << "File 1: " << char1 << std::endl;
            std::cout << "File 2: " << char2 << std::endl;

            // Print only the next 5 characters after the difference
            std::cout << "Next 5 characters after the difference: ";
            for (size_t i = 0; i < 5; ++i)
            {
                if (file2.get(char2))
                {
                    std::cout << char2;
                }
            }
            std::cout << std::endl;

            // Close files and return after finding the first difference
            file1.close();
            file2.close();
            return;
        }

        // Move to the next character
        ++charNumber;
    }

    // Check for errors after the loop
    if (file1.fail() || file2.fail())
    {
        std::cerr << "Error reading files: " << strerror(errno) << std::endl;
    }

    // Get file positions to check for different lengths
    std::streampos file1Size = file1.tellg();
    std::streampos file2Size = file2.tellg();

    if (file1Size == -1 || file2Size == -1)
    {
        // Handle error if unable to get file positions
        std::cerr << "Error getting file positions." << std::endl;
    }
    else if (file1Size != file2Size)
    {
        // Files have different lengths
        std::cout << "Files have different lengths." << std::endl;
    }
    else
    {
        // Files are identical
        std::cout << "The files are identical." << std::endl;
    }

    // Close the files at the end
    file1.close();
    file2.close();
}

int main()
{
    std::string file1Path = "./bm_test.txt.enc", file2Path = "./Test_Cases/large_files/encrypted_k=5/10.5.20.129.5000.enc";

    // Call the function to compare files
    compareFiles(file1Path, file2Path);

    return 0;
}
