#!/usr/bin/env python3

import re
# Read atomic positions and counts from the POSCAR file
with open("POSCAR", "r") as poscar_file:
    poscar_lines = poscar_file.readlines()
    atom_types = poscar_lines[5].split()
    atom_counts = list(map(int, poscar_lines[6].split()))

# Read original charges (ZVAL) from the POTCAR file
original_charges = {}

with open("POTCAR", "r") as potcar_file:
    lines = potcar_file.readlines()

    # Iterate over each line in the POTCAR file
    i = 0
    while i < len(lines):
        line = lines[i]

        # Check if the line contains 'VRHFIN ='
        if "VRHFIN =" in line:
            # Extract the atom type using regular expression
            match = re.search(r'VRHFIN\s*=\s*(\w+)\s*:', line)
            if match:
                atom_type = match.group(1)

                # Look for the next lines containing 'ZVAL' for the same atom type
                while i < len(lines):
                    i += 1
                    next_line = lines[i]

                    # Check if the next line contains 'ZVAL'
                    if "ZVAL" in next_line:
                        # Extract the absolute charge for this atom type
                        original_charge = float(next_line.split()[5])
                        original_charges[atom_type] = original_charge
                        break  # Move to the next 'VRHFIN'

        i += 1  # Move to the next line

# Read atomic charges from the ACF.dat file
with open("ACF.dat", "r") as acf_file:
    acf_lines = acf_file.readlines()

# Skip header lines and lines starting with '#'
acf_data_lines = [line.strip() for line in acf_lines if line.strip() and not line.startswith("#")]

# Read atomic charges
atomic_charges = []
for line in acf_data_lines:
    fields = line.split()
    try:
        charge = float(fields[4])  # Extract charge from the fourth column (index 4)
        atomic_charges.append(charge)
    except (IndexError, ValueError):
        print('No chrage in this line:"', line, '". Skip it!')

# Write net charges to a file
with open("Net_bader_charge.dat", "w") as output_file:
    output_file.write("#Index Symbole Net_charge\n")
    # Assign charges to atoms based on the counts and types, and calculate net charges
    atom_index = 0
    for atom_type, atom_count in zip(atom_types, atom_counts):
        original_charge = original_charges.get(atom_type)
        if original_charge is None:
            output_file.write(f"Error: Original charge not found for atom type '{atom_type}'\n")
            continue

        for _ in range(atom_count):
            atom_index += 1
            if atom_index <= len(atomic_charges):
                atom_charge = atomic_charges[atom_index - 1]  # Match the charge with the corresponding atom
                net_charge = original_charge - atom_charge  # Subtract the atomic charge from the original charge
                output_file.write(f"{atom_index} {atom_type} {net_charge}\n")
            else:
                output_file.write("Error: Insufficient atomic charges provided for atoms.\n")
                break

# Print success message
print("Net charges calculated and saved successfully in the file 'Net_bader_charge.dat'.")

# Define a function to calculate the sum of net charges of different atoms
def calculate_net_charge(selected_atoms):
    # Convert selected atoms to a list of indices
    indices = []
    for part in selected_atoms.split(','):
        if '-' in part:
            start, end = map(int, part.split('-'))
            indices.extend(range(start, end + 1))
        else:
            indices.append(int(part))

    # Read atomic positions and counts from the POSCAR file
    with open("POSCAR", "r") as poscar_file:
        poscar_lines = poscar_file.readlines()
        atom_types = poscar_lines[5].split()
        total_atoms = sum(map(int, poscar_lines[6].split()))

    # Check if all selected indices are within the range
    max_index = total_atoms
    invalid_indices = [index for index in indices if index < 1 or index > max_index]
    if invalid_indices:
        print("Error: Selected atom indices are out of range.")
        print("Valid indices are from 1 to", max_index)
        print("Invalid indices:", invalid_indices)
        return None

    # Read Net_bader_charge.dat and calculate sum of net charges for selected atoms
    sum_charge = 0.0
    selected_atoms_list = []
    with open("Net_bader_charge.dat", "r") as charge_file:
        for line in charge_file:
            if line.startswith ("#"):
                continue
            parts = line.split()
            atom_index = int(parts[0])
            if atom_index in indices:
                sum_charge += float(parts[-1])  # Last part contains the net charge
                selected_atoms_list.append((atom_index, parts[1]))  # Atom index and type

    return sum_charge, selected_atoms_list

# Example usage:
selected_atoms = input("Enter the selected atom indices (e.g., '1,2,5' or '2-6'): ")
result = calculate_net_charge(selected_atoms)
if result is not None:
    total_charge, selected_atoms_list = result
    print("Total net charge for selected atoms:", total_charge)
    print("Atoms summed:", selected_atoms_list)


