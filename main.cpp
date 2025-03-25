#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>

using namespace std;

struct RecordHeader {
    uint16_t opcode;
    uint16_t length;
};

struct RecordsID {
    std::vector<char>  ID;
};

struct Polys {
    std::vector<char>  ID;
    int16_t  ColorNameIndex;
    int16_t  materialIndex;
};

uint16_t be16_to_cpu(uint16_t be) {
    auto* bytes = (uint8_t*)&be;
    return (bytes[0] << 8) | bytes[1];
}

int16_t char_to_int_be(const char* bytes) {;
    return (bytes[0] << 8) | bytes[1];
}

void read_flt_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::vector<RecordsID> idRecords{};
    std::vector<Polys> polysVector{};
    while (true) {
        RecordHeader header{};
        file.read(reinterpret_cast<char*>(&header), sizeof(header));
        uint16_t opcode = be16_to_cpu(header.opcode);
        uint16_t length = be16_to_cpu(header.length);

        uint32_t data_length = length-sizeof(header);
        std::vector<char> data(data_length);
        if (data_length > 0) {
            file.read(data.data(), data_length);
        }
        if(opcode==1 || opcode==2 || opcode==4)
        {
            RecordsID record{};
            record.ID=std::vector<char>(data.begin(), data.begin() + 7);
            idRecords.push_back(record);
        }
        if(opcode==5)
        {
            Polys poly{};
            poly.ID=std::vector<char>(data.begin(), data.begin() + 7);
            poly.ColorNameIndex=char_to_int_be(std::vector<char>(data.begin() + 16, data.begin() + 18).data());
            poly.materialIndex=char_to_int_be(std::vector<char>(data.begin() + 26, data.begin() + 28).data());
            polysVector.push_back(poly);
        }
        if (file.eof()) break;
    }
    for(const auto& record:idRecords)
    {
        std::cout<<"ID ";
        for (char c : record.ID) {
            std::cout << c;
        }
        std::cout<<std::endl;
    }
    for(const auto& poly:polysVector)
    {
        std::cout<<"ID ";
        for (char c : poly.ID) {
            std::cout << c;
        }
        std::cout << " Color name index " << poly.ColorNameIndex << " material index " ;
        std::cout << poly.materialIndex;
        std::cout<<std::endl;
    }

    file.close();
}

int main() {
    read_flt_file("Model_3_ver164.flt");
    return 0;
}
