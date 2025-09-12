# Robot-Odometry-PID-Control

Mappare porta seriale su virtual machine (VirtualBox):
vai su Impostazioni -> PorteSeriali,
abilita porta seriale,
puoi fare fino a 4 mapping: selezionane una (es: COM1)
Modalità porta: Pipe host,
checkka "Connetti alla pipe/socket esistente",
Su Percorso/Indirizzo inserisci la porta della macchina host (porta da mappare) (es: su Windows se è COM10 --> \\.\COM10)
Ora la porta verrà mappata sulla porta COM1 della macchina guest (macchina virtuale) (c'è scritto COM1 ma se la macchina è Linux la si leggerà come ttyS0),
riavvia macchina virtuale.

NOTA:
se si interrompe la connessione bluetooth (ad esempio rebootando l'esp quando si riflasha il codice su di esso)
poi la connessione bluetooth si ricrea automaticamente ma la pipe si è rotta e quindi sulla porta mappata sulla VM non si leggerà
più nulla.
Bisogna prima caricare il codice sull'esp e poi startare l'intera VM per evitarlo.


Mappare dispositivo USB su virtual machine (VirtualBox):
vai su Impostazioni -> USB,
aggiungi nuovo filtro USB (icona +) selezionando il dispositivo,
riavvia macchina virtuale.

-------------------------------

Ricordati di creare la cartella build/ quando cloni da github