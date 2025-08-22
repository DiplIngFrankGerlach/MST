Minimales Chiffriersystem
•	Übersicht: Eine Vielzahl moderner Systeme muss über Funkverbindungen oder unsichere Fernmeldenetze ferngesteuert werden. 
Diese Fernsteuer-Schnittstellen sind bislang vielfach schlecht gesichert. Mit Hilfe eines einzigen, minimalen, beweisbar 
sicheren Chiffriersystems können alle diese Systeme sicher ferngesteuert werden.

•	Bietet 
o	Vertraulichkeit
o	Authentizität
o	unterschiedliche Verschlüsselung identischer Klartexte
o	verhindert Abspielangriffe(Replay Attack)

•	Rein symmetrisches Chiffriersystem(auf Grundlage von z.B. AES, 3DES, LIBELLE)

•	Minimaler Umfang von weniger als 2000 Zeilen C++. 

•	Aufgrund minimalem Umfangs mathematisch als korrekt beweisbar. Im Gegensatz dazu sind OpenSSH und OpenSSL 80 bis 200 mal größer und sehr schwer als korrekt(=sicher) zu beweisen.

•	Ideale Applikationen:
o	Satelliten-Steuerung
o	Industrielle Fernsteuerung von Prozessrechnern, SPS usw.
o	Steuerung von Netz-Komponenten in Stromnetz, Gasnetz, Wassernetz
o	Management-Fernsteuer-Schnittstellen von 

	Firewalls
	Server-Rechner im Rechenzentrum
	Router
	Switches
o	Ferngesteuerte Rechner-Desktops (MS RDP und X11)
