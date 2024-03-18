#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <regex>

// Function to read atomic positions and counts from the POSCAR file
void read_POSCAR(std::vector<std::string>& atom_types, std::vector<int>& atom_counts) {
    std::ifstream poscar_file("POSCAR");
    std::string line;

    // Read the POSCAR file line by line
    for (int i = 0; i < 7; ++i) {
        std::getline(poscar_file, line);
        if (i == 5) {
            std::istringstream iss(line);
            std::string type;
            while (iss >> type) {
                atom_types.push_back(type);
            }
        }
        else if (i == 6) {
            std::istringstream iss(line);
            int count;
            while (iss >> count) {
                atom_counts.push_back(count);
            }
        }
    }
    poscar_file.close();
}

// Function to read original charges (ZVAL) from the POTCAR file
std::map<std::string, double> read_POTCAR() {
    std::ifstream potcar_file("POTCAR");
    std::map<std::string, double> original_charges;
    std::string line;

    // Read the POTCAR file line by line
    while (std::getline(potcar_file, line)) {
        if (line.find("VRHFIN =") != std::string::npos) {
            std::regex regex_pattern("VRHFIN\\s*=\\s*(\\w+)\\s*:");
            std::smatch match;
            if (std::regex_search(line, match, regex_pattern)) {
                std::string atom_type = match[1];
                double original_charge = 0.0;

                // Look for the next lines containing 'ZVAL' for the same atom type
                while (std::getline(potcar_file, line)) {
                    if (line.find("ZVAL") != std::string::npos) {
                        std::istringstream iss(line);
                        std::string token;
                        for (int i = 0; i < 6; ++i) {
                            iss >> token; // Skip tokens until the charge value
                        }
                        try {
                            original_charge = std::stod(token);
                            original_charges[atom_type] = original_charge;
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Error converting string to double: " << token << std::endl;
                            // Handle the error, such as skipping this entry or setting a default value
                        }
                        break; // Move to the next 'VRHFIN'
                    }
                }
            }
        }
    }
    potcar_file.close();
    return original_charges;
}



// Function to read atomic charges from the ACF.dat file
std::vector<double> read_ACF_dat() {
    std::ifstream acf_file("ACF.dat");
    std::vector<double> atomic_charges;
    std::string line;

    // Read the ACF.dat file line by line
    while (std::getline(acf_file, line)) {
        if (!line.empty() && line[0] != '#') {
            std::istringstream iss(line);
            std::string field;
            for (int i = 0; i < 4; ++i) {
                iss >> field; // Skipping the first 4 fields
            }
            double charge;
            if (iss >> charge) {
                atomic_charges.push_back(charge);
            }
        }
    }
    acf_file.close();
    return atomic_charges;
}

// Function to write net charges to a file
void write_net_charges(const std::vector<std::string>& atom_types, const std::vector<int>& atom_counts,
                       const std::map<std::string, double>& original_charges, const std::vector<double>& atomic_charges) {
    std::ofstream output_file("Net_bader_charge.dat");
    output_file << "#Index Symbole Net_charge\n";

    int atom_index = 0;
    for (size_t i = 0; i < atom_types.size(); ++i) {
        std::string atom_type = atom_types[i];
        int atom_count = atom_counts[i];
        double original_charge = 0.0;

        if (original_charges.count(atom_type) > 0) {
            original_charge = original_charges.at(atom_type);
        } else {
            std::cerr << "Error: Original charge not found for atom type '" << atom_type << "'" << std::endl;
            // Handle the error, such as skipping this entry or setting a default value
        }

        for (int j = 0; j < atom_count; ++j) {
            ++atom_index;
            if (atom_index <= atomic_charges.size()) {
                double atom_charge = atomic_charges[atom_index - 1];
                double net_charge = original_charge - atom_charge;
                output_file << atom_index << " " << atom_type << " " << net_charge << "\n";
            }
            else {
                output_file << "Error: Insufficient atomic charges provided for atoms.\n";
                break;
            }
        }
    }
    output_file.close();
}

int main() {
    std::vector<std::string> atom_types;
    std::vector<int> atom_counts;
    std::map<std::string, double> original_charges;
    std::vector<double> atomic_charges;

    // Read from files
    read_POSCAR(atom_types, atom_counts);
    original_charges = read_POTCAR();
    atomic_charges = read_ACF_dat();

    // Write net charges to a file
    write_net_charges(atom_types, atom_counts, original_charges, atomic_charges);

    // Print success message
    std::cout << "Net charges calculated and saved successfully in the file 'Net_bader_charge.dat'.\n";

    return 0;
}

