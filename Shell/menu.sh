#!/bin/bash

echo "1. Gestisci sessione"
echo "2. Esegui operazione"
echo "3. Mostra cronologia sessione"
echo "4. Richiama operazione precedente"
echo "5. Cerca in cronologia sessione"
echo "6. Esci"

#chiedo all'utente di fare un input da tastiera
echo -e "\nInserisci il numero dell'operazione"
read sceltaMenu

case $sceltaMenu in
1)echo "Hai scelto la 1"; ./sessione.sh;;
2)echo "Hai scelto la 2"; ./operazione.sh $1;;
3)echo "Hai scelto la 3"; ./cronologia.sh $1;;
4)echo "Hai scelto la 4"; ./precedente.sh $1;;
5)echo "Hai scelto la 5"; ./cercaCronologia.sh $1;;
6)./esci.sh;;
esac
