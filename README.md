# bader4vasp  
C++ implementation of Bader analysis charge extraction from VASP output

# Description of the code:  
This code can be used to calculate the net charge of each atom of your system after applying bader analaysis on VASP calculation:  
1-Reads Atomic Positions and Counts: The code reads the atomic positions and counts from the POSCAR file, which typically contains information about the structure of a material or molecule.  
=2-Reads Original Charges (ZVAL) from POTCAR: It then reads the original charges of the atoms (ZVAL) from the POTCAR file. These original charges are typically provided by the user or calculated based on the atomic properties.
3-Reads Atomic Charges from ACF.dat: Next, the code reads the atomic charges from the ACF.dat file. These charges are usually obtained from the Bader analysis performed on the electronic density of the system.
4-Calculates Net Charges: Using the original charges and the atomic charges, the code calculates the net charges for each atom in the system based on the Bader analysis.
5-Writes Net Charges to a File: Finally, the code writes the calculated net charges to a file named "Net_bader_charge.dat", along with the corresponding atom indices and types.

# Installation steps:
1- Download the code "bader_vasp.cpp".
2- Comnpile it with "g++ -o bader_vasp bader_vasp.cpp" or other C++ compiler.
3- Run the excutable "./bader_vasp" in the folder containing the POSACR, the POTCAR and the ACE.dat files.
