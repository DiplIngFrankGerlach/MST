# Minimales Chiffriersystem / Minumum Secure Transport - MST

Autor: Dipl. Ing. Frank Gerlach

## Übersicht
Eine Vielzahl moderner Systeme muss über Funkverbindungen oder unsichere Fernmeldenetze ferngesteuert werden. 
Diese Fernsteuer-Schnittstellen sind bislang vielfach schlecht gesichert. Mit Hilfe eines einzigen, minimalen, beweisbar 
sicheren Chiffriersystems können alle diese Systeme sicher ferngesteuert werden.

## Garantien des Chiffriersystems
 - Vertraulichkeit
 - Authentizität
 - unterschiedliche Verschlüsselung identischer Klartexte
 - verhindert Abspielangriffe(Replay Attack)

## Chiffriertechnik
Rein symmetrisches Chiffriersystem(auf Grundlage von z.B. AES, 3DES, LIBELLE)

## Umfang
Minimaler Umfang von weniger als 2000 Zeilen C++. 

## Im Vergleich zu anderen Chiffriersystemen
Aufgrund minimalem Umfangs mathematisch als korrekt beweisbar. Im Gegensatz dazu sind OpenSSH und OpenSSL 
80 bis 200 mal größer und sehr schwer als korrekt(=sicher) zu beweisen. In der Vergangenheit hatten alle
SSL-Systeme schwerste Sicherheitslücken wie z.B. HEARTBLEED

## Ideale Applikationen:
- Satelliten-Steuerung
- Industrielle Fernsteuerung von Prozessrechnern, SPS usw.
- Steuerung von Netz-Komponenten in Stromnetz, Gasnetz, Wassernetz
- Management-Fernsteuer-Schnittstellen von
  - Firewalls
  - Server-Rechner im Rechenzentrum
  - Router
  - Switches
  - Ferngesteuerte Rechner-Desktops (MS RDP und X11)
