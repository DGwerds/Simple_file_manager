#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <limits>
#include <bits/stdc++.h>
#include <cctype>
using namespace std;

string charfig = "Configuracion.txt";
string ejecutable = "Organizador de archivos.exe";
string cpp = "Organizador de archivos.cpp";
fstream config(charfig);

string IrLinea(char num, int linea_plus = 1){
	config.seekg(ios::beg);
	string n_linea;
	bool encontrado = false;
	while(!config.eof()){
		getline(config, n_linea);
		if (linea_plus == 0){
			return n_linea;
		}
		if(n_linea[0] == num or encontrado == true){
			encontrado = true;
			linea_plus--;
		}
	}
    return "Error";
}

vector<string> filtrar_por_palabra(vector<string> lista_archivos_copia){
	vector<string> lista_archivos;
	vector<string> palabra_clave;
	string palabra;
	vector<string> salida;
	int similitud = 0;
	cout << endl << "Palabras clave: ";
	stringstream ss(IrLinea('3'));
    while (ss >> palabra){
    	palabra_clave.push_back(palabra);
    	cout << palabra << "  ";
	} cout << endl;
	
	for (int index = 0; index < palabra_clave.size(); index++){
		for (int letra = 0; letra < palabra_clave[index].length(); letra++){
			palabra_clave[index][letra] = tolower(palabra_clave[index][letra]);
		}
	}
	
	for (int index = 0; index < lista_archivos_copia.size(); index++){
		lista_archivos.push_back(lista_archivos_copia[index]);
	}
	
	for (int index = 0; index < lista_archivos_copia.size(); index++){
		for (int letra = 0; letra < lista_archivos_copia[index].length(); letra++){
			lista_archivos[index][letra] = tolower(lista_archivos[index][letra]);
		}
	}
	
	for (int index_buscar = 0; index_buscar < palabra_clave.size(); index_buscar++){
		salida.push_back("[" + palabra_clave[index_buscar] + "]");
		for (int index_lista = 0; index_lista < lista_archivos.size(); index_lista++){
			for (int letra_lista = 0; letra_lista < lista_archivos[index_lista].length(); letra_lista++){
				if (lista_archivos[index_lista][letra_lista] == palabra_clave[index_buscar][0]){
					for (int letra_buscar = 0; letra_buscar < palabra_clave[index_buscar].length(); letra_buscar++){
						if (palabra_clave[index_buscar][letra_buscar] == lista_archivos[index_lista][letra_lista+letra_buscar]){
							similitud++;
						}
					}
					if (similitud >= palabra_clave[index_buscar].length()){
						salida.push_back(lista_archivos_copia[index_lista]);
						letra_lista += lista_archivos_copia[index_lista].length();
					}
					similitud = 0;
				}
			}
		}
	}
	cout << "\nEncontrados por palabras clave";
	int total = 0;
	for (int i = 0; i < salida.size(); i++){
		if (salida[i][0] == '['){
			cout << endl << salida[i] << " = ";			
			total++;
		} else{
			cout << salida[i] << "  ";
		}
	} cout << endl << "total de archivos a mover = "<< salida.size()-total << endl <<endl;
	return salida;
}


vector<string> filtrados_por_extencion(){
	string carpeta = IrLinea('1');
	cout << "Carpeta a revisar: " << carpeta << endl;
	vector<string> nombres;
	vector<string> extenciones;
	string filtro_extenciones;
	stringstream ss(IrLinea('2'));
	cout << "Extenciones a buscar: ";
    while (ss >> filtro_extenciones){
    	if (filtro_extenciones != "*"){
    		extenciones.push_back(filtro_extenciones);
		} else{
			extenciones.clear();
			extenciones.push_back(filtro_extenciones);
			ss.str("");
			ss.clear();
		}
		cout << "." << filtro_extenciones << "  ";
	}
	cout << endl;
	
	WIN32_FIND_DATA fd;
	for (int index = 0; index < extenciones.size(); index++){
		string busqueda_extencion = carpeta + "\\" + "/*." + extenciones[index];
		int cantidad = 0;
		cout << "\nArchivos ." << extenciones[index] << endl;
		HANDLE hFind = ::FindFirstFile(busqueda_extencion.c_str(), &fd); 
		if(hFind != INVALID_HANDLE_VALUE){
			do {
				if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)){
					string nombre_archivo = fd.cFileName;
					if (nombre_archivo != charfig && nombre_archivo != ejecutable && nombre_archivo != cpp){
						nombres.push_back(nombre_archivo);
						cantidad++;
						cout << "	" << nombre_archivo << endl;
					}
				}
			} while(::FindNextFile(hFind, &fd)); 
			::FindClose(hFind); 
		}
	cout << "." << extenciones[index] << " encontrados: " << cantidad << endl;
	}
	cout << "\ntotal archivos encontrados: " << nombres.size() << endl;
    return nombres;
}

int mover_archivos(vector<string> archivos_a_mover){
	string carpeta_origen = IrLinea('1') +  "\\";
	int linea_plus = 0, movidos = 0, errores = 0;
	for (int cantidad = 0; cantidad < archivos_a_mover.size(); cantidad++){
		while (archivos_a_mover[cantidad][0] == '[' and cantidad < archivos_a_mover.size()-1){
			cantidad++;
			linea_plus++;
		}
		string carpeta_final = IrLinea('4', linea_plus) + "\\";
//		cout << cantidad << "  " << archivos_a_mover.size() << endl;

		string archivo = carpeta_origen + archivos_a_mover[cantidad];
		string archivo_final = carpeta_final + archivos_a_mover[cantidad];
	
		char viejo[carpeta_origen.length() + archivos_a_mover[cantidad].length() + 2];
		char nuevo[carpeta_final.length() + archivos_a_mover[cantidad].length() + 2];
	
		strcpy(viejo, archivo.c_str()); 
		strcpy(nuevo, archivo_final.c_str());

		if (rename(viejo, nuevo) != 0)
//			cout << "Error" << endl;
			errores++;
		else{
			cout << "Se a movido: " << archivos_a_mover[cantidad] << endl;
			movidos++;
		}
	}
	if (movidos != 0){
		cout << "\nSe han movido con exito " << movidos << " archivos" << endl;
	}
	if (errores != 0){
		cout << "\nNo han movido " << errores-1 << " archivos" << endl << \
			"Revisa que no existan archivos con el mismo nombre en la carpeta de destino" << endl;
	}
	return 0;
}

main(){
	mover_archivos(filtrar_por_palabra(filtrados_por_extencion()));
	cin.get();
}
