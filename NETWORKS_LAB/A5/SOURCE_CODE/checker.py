def main():
    with open('input.txt', 'r') as input_file, open('output.txt', 'r') as output_file:
        input_lines = input_file.readlines()
        output_lines = output_file.readlines()

        mismatch_found = False

        for i, (input_line, output_line) in enumerate(zip(input_lines, output_lines)):
            if input_line != output_line:
                mismatch_found = True
                print(f"Mismatch found at line {i + 1}:")
                print(f"Input:  {input_line.strip()}")
                print(f"Output: {output_line.strip()}")

                # Print 100 characters after the first mismatch
                pos = next((pos for pos, (a, b) in enumerate(zip(input_line, output_line)) if a != b), None)
                if pos is not None:
                    print("100 characters after mismatch:")
                    print(input_line[pos:pos+100])
                break

        if not mismatch_found:
            print("Files are identical.")

if __name__ == "__main__":
    main()
