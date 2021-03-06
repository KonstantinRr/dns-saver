#include <dnssaver/dnssaver.h>

using namespace std;

state textToBin(ifstream &in, ofstream &out)
{
    in.seekg(0, ios::end);  // seeks to the end of file
                            // gets the file size
    size_t size = static_cast<size_t>(in.tellg());
    in.seekg(0, ios::beg);            // seeks back to the beginning

    out.put(MAGIC); // write the magic byte at the beginning
                    // writes the amount of binary chars coming
    out.write(reinterpret_cast<char const *>(&size), sizeof(size));

    size_t shift = 0;   // the current offset in writeData
    char writeData = 0; // full byte that is written to out

    while (!in.eof())
    {
        char inChar;
        in.get(inChar); // reads the next char from in
        if (!in.eof() && in.fail())  // performs fast error checking
            return s_err_read_input;

        switch (inChar)
        {
            case 'A': break; // writes 0
            case 'C':
                writeData |= 1 << shift;
                break;
            case 'T':
                writeData |= 2 << shift;
                break;
            case 'G':
                writeData |= 3 << shift;
                break;
            default:
                // error code -5: invalid character
                return s_err_unknown_char;
        }

        shift += 2;         // moves the shift by two bits
        if (shift == 8)     // writeData is full and is written to out
        {
            out.put(writeData);
            if (out.fail()) // fast error checking
                // err -4: error writing output file
                return s_err_write_output;
            writeData = 0;
            shift = 0;
        }
    }

    if (shift != 0)         // write the remaining data to out
    {
        out.put(writeData);
        if (out.fail())     // fast error checking
            // err -4: error writing output file
            s_err_write_output;
    }
    return s_success;
}
