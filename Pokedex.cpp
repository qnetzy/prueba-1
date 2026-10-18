#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

struct DataMon {
	int id_num;
	string apelativo;
	string cat_uno;
	string cat_dos;
	int puntaje_total;
	int puntos_vida;
	int fuerza;
	int blindaje;
	int fuerza_magica;
	int blindaje_magico;
	int agilidad;
	int epoca;
	bool mistico;
};

const vector<string> lista_etiquetas = {
	"Nombre", "Tipo 1", "Tipo 2", "Numero Pokedex", "Total", "HP",
	"Ataque", "Defensa", "Ataque especial", "Defensa especial",
	"Velocidad", "Generacion", "Legendario"
};

vector<string> parsearCSV(const string& text_linea) {
	vector<string> columnas;
	string fragmento;
	bool comillas_activas = false;

	for (size_t idx = 0; idx < text_linea.size(); ++idx) {
		char letra = text_linea[idx];
		if (letra == '"') {
			if (comillas_activas && idx + 1 < text_linea.size() && text_linea[idx + 1] == '"') {
				fragmento += '"';
				++idx;
			} else {
				comillas_activas = !comillas_activas;
			}
		} else if (letra == ',' && !comillas_activas) {
			columnas.push_back(fragmento);
			fragmento.clear();
		} else {
			fragmento += letra;
		}
	}
	columnas.push_back(fragmento);
	return columnas;
}

bool importarMonstruos(const string& source_path, vector<DataMon>& coleccion) {
	ifstream archivo_datos(source_path.c_str());
	if (!archivo_datos.is_open()) {
		return false;
	}

	string text_linea;
	getline(archivo_datos, text_linea);
	while (getline(archivo_datos, text_linea)) {
		if (!text_linea.empty() && text_linea[text_linea.size() - 1] == '\r') {
			text_linea.erase(text_linea.size() - 1);
		}
		vector<string> columnas = parsearCSV(text_linea);
		if (columnas.size() != 13) {
			continue;
		}

		try {
			DataMon item;
			item.id_num = stoi(columnas[0]);
			item.apelativo = columnas[1];
			item.cat_uno = columnas[2];
			item.cat_dos = columnas[3];
			item.puntaje_total = stoi(columnas[4]);
			item.puntos_vida = stoi(columnas[5]);
			item.fuerza = stoi(columnas[6]);
			item.blindaje = stoi(columnas[7]);
			item.fuerza_magica = stoi(columnas[8]);
			item.blindaje_magico = stoi(columnas[9]);
			item.agilidad = stoi(columnas[10]);
			item.epoca = stoi(columnas[11]);
			item.mistico = columnas[12] == "True" || columnas[12] == "true";
			coleccion.push_back(item);
		} catch (const exception&) {
			continue;
		}
	}
	return true;
}

string pasarAMinusculas(string cadena) {
	transform(cadena.begin(), cadena.end(), cadena.begin(), [](unsigned char caracter) {
		return static_cast<char>(tolower(caracter));
	});
	return cadena;
}

int solicitarEntero(const string& prompt_msg, int limite_inf, int limite_sup) {
	while (true) {
		cout << prompt_msg;
		string input_usuario;
		getline(cin, input_usuario);
		stringstream stream_val(input_usuario);
		int resultado_num;
		char extra_char;
		if (stream_val >> resultado_num && !(stream_val >> extra_char) && resultado_num >= limite_inf && resultado_num <= limite_sup) {
			return resultado_num;
		}
		cout << "Ingresa un numero entre " << limite_inf << " y " << limite_sup << ".\n";
	}
}

string solicitarTexto(const string& prompt_msg) {
	cout << prompt_msg;
	string input_str;
	getline(cin, input_str);
	return input_str;
}

void limpiarPantallaConsola() {
	system("cls");
}

int obtenerValorNumerico(const DataMon& element, int idx_campo) {
	switch (idx_campo) {
		case 3: return element.id_num;
		case 4: return element.puntaje_total;
		case 5: return element.puntos_vida;
		case 6: return element.fuerza;
		case 7: return element.blindaje;
		case 8: return element.fuerza_magica;
		case 9: return element.blindaje_magico;
		case 10: return element.agilidad;
		default: return element.epoca;
	}
}

string obtenerValorTexto(const DataMon& element, int idx_campo) {
	switch (idx_campo) {
		case 0: return element.apelativo;
		case 1: return element.cat_uno;
		case 2: return element.cat_dos;
		default: return element.mistico ? "True" : "False";
	}
}

void desplegarCampos() {
	for (size_t idx = 0; idx < lista_etiquetas.size(); ++idx) {
		cout << idx + 1 << ". " << lista_etiquetas[idx] << "\n";
	}
}

void renderizarGrilla(const vector<DataMon>& coleccion) {
	cout << left << setw(6) << "Nro" << setw(16) << "Nombre" << setw(12) << "Tipo 1"
		 << setw(12) << "Tipo 2" << right << setw(7) << "Total" << setw(6) << "HP"
		 << setw(9) << "Ataque" << setw(10) << "Defensa" << setw(10) << "Atq.Esp"
		 << setw(10) << "Def.Esp" << setw(10) << "Velocidad" << setw(6) << "Gen"
		 << setw(12) << "Legendario" << "\n";
	cout << string(126, '-') << "\n";

	for (size_t idx = 0; idx < coleccion.size(); ++idx) {
		const DataMon& current = coleccion[idx];
		cout << left << setw(6) << current.id_num << setw(16) << current.apelativo
			 << setw(12) << (current.cat_uno.empty() ? "-" : current.cat_uno)
			 << setw(12) << (current.cat_dos.empty() ? "-" : current.cat_dos)
			 << right << setw(7) << current.puntaje_total << setw(6) << current.puntos_vida << setw(9) << current.fuerza
			 << setw(10) << current.blindaje << setw(10) << current.fuerza_magica
			 << setw(10) << current.blindaje_magico << setw(10) << current.agilidad
			 << setw(6) << current.epoca << setw(12) << (current.mistico ? "Si" : "No") << "\n";
	}
	cout << "Resultados: " << coleccion.size() << "\n";
}

void ejecutarFiltro(const vector<DataMon>& coleccion) {
	desplegarCampos();
	int idx_campo = solicitarEntero("Campo para filtrar: ", 1, 13) - 1;
	vector<DataMon> filtrados;

	if (idx_campo == 0) {
		string query = pasarAMinusculas(solicitarTexto("Texto a buscar (puede ser parcial): "));
		for (size_t idx = 0; idx < coleccion.size(); ++idx) {
			if (pasarAMinusculas(obtenerValorTexto(coleccion[idx], idx_campo)).find(query) != string::npos) {
				filtrados.push_back(coleccion[idx]);
			}
		}
	} else if (idx_campo == 1 || idx_campo == 2) {
		vector<string> alternativas;
		for (size_t idx = 0; idx < coleccion.size(); ++idx) {
			string tipo_val = obtenerValorTexto(coleccion[idx], idx_campo);
			if (find(alternativas.begin(), alternativas.end(), tipo_val) == alternativas.end()) {
				alternativas.push_back(tipo_val);
			}
		}
		sort(alternativas.begin(), alternativas.end(), [](const string& val_a, const string& val_b) {
			return pasarAMinusculas(val_a) < pasarAMinusculas(val_b);
		});
		for (size_t idx = 0; idx < alternativas.size(); ++idx) {
			cout << idx + 1 << ". " << (alternativas[idx].empty() ? "Sin tipo secundario" : alternativas[idx]) << "\n";
		}
		int seleccion_tipo = solicitarEntero("Elige un tipo: ", 1, static_cast<int>(alternativas.size())) - 1;
		for (size_t idx = 0; idx < coleccion.size(); ++idx) {
			if (obtenerValorTexto(coleccion[idx], idx_campo) == alternativas[seleccion_tipo]) {
				filtrados.push_back(coleccion[idx]);
			}
		}
	} else if (idx_campo == 12) {
		cout << "1. Legendario\n2. No legendario\n";
		int opcion_leg = solicitarEntero("Elige una opcion: ", 1, 2);
		bool buscar_mistico = opcion_leg == 1;
		for (size_t idx = 0; idx < coleccion.size(); ++idx) {
			if (coleccion[idx].mistico == buscar_mistico) {
				filtrados.push_back(coleccion[idx]);
			}
		}
	} else {
		cout << "Usa el mismo minimo y maximo para buscar un valor exacto.\n";
		int lim_inf = solicitarEntero("Valor minimo: ", 0, 10000);
		int lim_sup = solicitarEntero("Valor maximo: ", lim_inf, 10000);
		for (size_t idx = 0; idx < coleccion.size(); ++idx) {
			int val_num = obtenerValorNumerico(coleccion[idx], idx_campo);
			if (val_num >= lim_inf && val_num <= lim_sup) {
				filtrados.push_back(coleccion[idx]);
			}
		}
	}
	renderizarGrilla(filtrados);
}

void ejecutarOrdenamiento(const vector<DataMon>& coleccion) {
	desplegarCampos();
	int idx_campo = solicitarEntero("Ordenar por campo: ", 1, 13) - 1;
	int sentido = solicitarEntero("1. Menor a mayor / A-Z\n2. Mayor a menor / Z-A\nOpcion: ", 1, 2);
	vector<DataMon> clon_ordenado = coleccion;

	stable_sort(clon_ordenado.begin(), clon_ordenado.end(), [idx_campo, sentido](const DataMon& val_a, const DataMon& val_b) {
		bool es_menor;
		if (idx_campo <= 2 || idx_campo == 12) {
			es_menor = pasarAMinusculas(obtenerValorTexto(val_a, idx_campo)) < pasarAMinusculas(obtenerValorTexto(val_b, idx_campo));
		} else {
			es_menor = obtenerValorNumerico(val_a, idx_campo) < obtenerValorNumerico(val_b, idx_campo);
		}
		return sentido == 1 ? es_menor : !es_menor &&
			(idx_campo <= 2 || idx_campo == 12
				? pasarAMinusculas(obtenerValorTexto(val_a, idx_campo)) != pasarAMinusculas(obtenerValorTexto(val_b, idx_campo))
				: obtenerValorNumerico(val_a, idx_campo) != obtenerValorNumerico(val_b, idx_campo));
	});
	renderizarGrilla(clon_ordenado);
}

void mostrarRankingExtremos(const vector<DataMon>& coleccion) {
	vector<DataMon> clon_ordenado = coleccion;
	stable_sort(clon_ordenado.begin(), clon_ordenado.end(), [](const DataMon& val_a, const DataMon& val_b) {
		return val_a.puntaje_total < val_b.puntaje_total;
	});

	size_t limite = min(static_cast<size_t>(3), clon_ordenado.size());
	vector<DataMon> bloque_debil;
	vector<DataMon> bloque_fuerte;
	for (size_t idx = 0; idx < limite; ++idx) {
		bloque_debil.push_back(clon_ordenado[idx]);
		bloque_fuerte.push_back(clon_ordenado[clon_ordenado.size() - 1 - idx]);
	}

	cout << "Top 3 Pokemon mas debiles (menor Total):\n";
	renderizarGrilla(bloque_debil);
	cout << "\nTop 3 Pokemon mas fuertes (mayor Total):\n";
	renderizarGrilla(bloque_fuerte);
}

const DataMon* buscarItemPorID(const vector<DataMon>& coleccion, int id_buscado) {
	for (size_t idx = 0; idx < coleccion.size(); ++idx) {
		if (coleccion[idx].id_num == id_buscado) {
			return &coleccion[idx];
		}
	}
	return NULL;
}

void ejecutarComparativa(const vector<DataMon>& coleccion) {
	int id_primero = solicitarEntero("Numero Pokedex del primer Pokemon: ", 1, 150);
	int id_segundo = solicitarEntero("Numero Pokedex del segundo Pokemon: ", 1, 150);
	const DataMon* sujeto_a = buscarItemPorID(coleccion, id_primero);
	const DataMon* sujeto_b = buscarItemPorID(coleccion, id_segundo);

	if (sujeto_a == NULL || sujeto_b == NULL) {
		cout << "No se encontro uno de los Pokemon en el CSV.\n";
		return;
	}

	vector<DataMon> par_seleccionado;
	par_seleccionado.push_back(*sujeto_a);
	par_seleccionado.push_back(*sujeto_b);
	renderizarGrilla(par_seleccionado);
	cout << "Comparacion de stats (gana el valor mas alto):\n";
	const int arreglo_stats[] = {4, 5, 6, 7, 8, 9, 10};
	for (size_t idx = 0; idx < sizeof(arreglo_stats) / sizeof(arreglo_stats[0]); ++idx) {
		int idx_campo = arreglo_stats[idx];
		int puntaje_a = obtenerValorNumerico(*sujeto_a, idx_campo);
		int puntaje_b = obtenerValorNumerico(*sujeto_b, idx_campo);
		cout << lista_etiquetas[idx_campo] << ": ";
		if (puntaje_a == puntaje_b) {
			cout << "empate (" << puntaje_a << ")\n";
		} else if (puntaje_a > puntaje_b) {
			cout << sujeto_a->apelativo << " (" << puntaje_a << ")\n";
		} else {
			cout << sujeto_b->apelativo << " (" << puntaje_b << ")\n";
		}
	}
}

int main() {
	vector<DataMon> base_datos;
	string ruta_archivo = "pokemon_150_registros.csv";
	if (!importarMonstruos(ruta_archivo, base_datos)) {
		cout << "No encontre " << ruta_archivo << " en la carpeta actual.\n";
		ruta_archivo = solicitarTexto("Escribe la ruta del archivo CSV: ");
		if (!importarMonstruos(ruta_archivo, base_datos)) {
			cout << "No se pudo abrir el archivo.\n";
			return 1;
		}
	}
	if (base_datos.empty()) {
		cout << "El CSV no contiene registros validos.\n";
		return 1;
	}

	while (true) {
		limpiarPantallaConsola();
		cout << "Se cargaron " << base_datos.size() << " Pokemon.\n";
		cout << "\n--- POKEDEX ---\n"
			 << "1. Mostrar todos\n"
			 << "2. Filtrar por campo\n"
			 << "3. Ordenar por campo\n"
			 << "4. Comparar dos Pokemon\n"
			 << "5. Ver Top 3 fuertes y debiles\n"
			 << "6. Salir\n";
		int opcion_menu = solicitarEntero("Opcion: ", 1, 6);
		limpiarPantallaConsola();
		if (opcion_menu == 1) {
			renderizarGrilla(base_datos);
		} else if (opcion_menu == 2) {
			ejecutarFiltro(base_datos);
		} else if (opcion_menu == 3) {
			ejecutarOrdenamiento(base_datos);
		} else if (opcion_menu == 4) {
			ejecutarComparativa(base_datos);
		} else if (opcion_menu == 5) {
			mostrarRankingExtremos(base_datos);
		} else {
			break;
		}
		cout << "\nPresiona Enter para volver al menu...";
		string buffer_pausa;
		getline(cin, buffer_pausa);
	}
	return 0;
}