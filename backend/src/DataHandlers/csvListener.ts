import fs from "fs";
import csv from "csv-parser";


// Pfad zur CSV-Datei
const csvFilePath = '../csvdummy/data.csv';

// Aktuelle Anzahl der Zeilen in der CSV-Datei
let currentRowCount = 0;

// CSV-Listener-Funktion


export async function csvListener() {
    while (true) {
      await new Promise<void>((resolve) => {
        const fileStream = fs.createReadStream(csvFilePath);
        fileStream.pipe(csv())
          .on('data', (row) => {
            // Prüfe, ob die Anzahl der Zeilen größer als die aktuelle Anzahl ist
            if (++currentRowCount > 1) {
              console.log("___________________________________________________")
              console.log('Neue Zeile in der CSV-Datei:', row);
              console.log("___________________________________________________")
             
            }
          })
          .on('end', () => {
            console.log('CSV-Datei komplett gelesen');
            resolve(); // Auflösen der Promise, um die nächste Iteration zu starten
          });
      });
      // Warte eine Weile, bevor die nächste Iteration beginnt
      await new Promise((resolve) => setTimeout(resolve, 3000)); // z.B. 3000 Millisekunden (3 Sekunden)
    }
  }