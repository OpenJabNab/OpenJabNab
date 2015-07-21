<?php
$reseau = array("rer" => array("RA", "RB"),
				"metro" => array("1", "2","3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14"), 
				"tram" => array("T1", "T2", "T3"));
$direction = array("RA" => array("R" => "Marne la Vallee-Boissy Saint Leger", "A" => "Cergy-Poissy-Saint Germain en Laye"),
					"RB" => array("R" => "Robinson-Saint Remy les Chevreuse", "A" => "Aeroport Ch.de Gaulle 2-Mitry Claye"),
					"1" => array("R" => "La Défense", "A" => "Nation"),
					"2" => array("R" => "Nation", "A" => "Dauphine"),
					"3" => array("R" => "Pont de Levallois Bécon", "A" => "Gallieni"),
					"4" => array("R" => "Porte de Clignancourt", "A" => "Porte d'Orléans"),
					"5" => array("R" => "Bobigny Pablo Picasso", "A" => "Place d'Italie"),
					"6" => array("R" => "Nation", "A" => "Charles de Gaulle Etoile"),
					"7" => array("R" => "La Courneuve 8 mai 1945", "A" => "Mairie d'Ivry - Villejuif Louis Aragon"),
					"8" => array("R" => "Balard", "A" => "Créteil-Préfecture"),
					"9" => array("R" => "Pont de Sèvres", "A" => "Mairie de Montreuil"),
					"10" => array("R" => "Boulogne Pont de Saint-Cloud", "A" => "Gare d'Austerlitz"),
					"11" => array("R" => "Mairie des Lilas", "A" => "Châtelet"),
					"12" => array("R" => "Mairie d'Issy", "A" => "Porte de la Chapelle"),
					"13" => array("R" => "Châtillon-Montrouge", "A" => "Asnières-Gennevilliers Les Courtilles - Saint-Denis-Université"),
					"14" => array("R" => "Olympiades", "A" => "Saint-Lazare"),
					"T1" => array("R" => "Gare de Noisy le Sec", "A" => "Gare de St Denis"),
					"T2" => array("R" => "Porte de Versailles", "A" => "La Défense"),
					"T3" => array("R" => "Pont du Garigliano", "A" => "Porte Ivry"));			

$arret = array("T1" => array("T1_326_327" => "AUGUSTE DELAUNE",
"T1_15_49" => "BASILIQUE DE ST DENIS",
"T1_1_4" => "BOBIGNY - PABLO PICASSO",
"T1_16_48" => "CIMETIERE DE ST DENIS",
"T1_18_46" => "COSMONAUTES",
"T1_23_42" => "DANTON",
"T1_26_39" => "DRANCY AVENIR",
"T1_29_36" => "ESCADRILLE NORMANDIE-NIEMEN",
"T1_334_337" => "GARE DE NOISY-LE-SEC",
"T1_28_37" => "GASTON ROULAUD",
"T1_27_38" => "HOPITAL AVICENNE",
"T1_17_47" => "HOPITAL DELAFONTAINE",
"T1_32_33" => "HOTEL DE VILLE DE BOBIGNY",
"T1_20_44" => "HOTEL DE VILLE LA COURNEUVE",
"T1_585_784" => "JEAN ROSTAND",
"T1_19_45" => "LA COURNEUVE 6 ROUTES",
"T1_24_41" => "LA COURNEUVE 8 MAI 1945",
"T1_30_35" => "LA FERME",
"T1_31_34" => "LIBERATION",
"T1_14_50" => "MARCHE DE ST DENIS",
"T1_25_40" => "MAURICE LACHATRE",
"T1_333_336" => "PETIT NOISY",
"T1_332_335" => "PONT DE BONDY",
"T1_52_53" => "SAINT-DENIS",
"T1_22_43" => "STADE GEO ANDRE",
"T1_13_51" => "THEATRE G. PHILIPE"),
"T2" => array("T2_60103_60203" => "BELVEDERE",
"T2_60109_60209" => "BRIMBORION",
"T2_570_571" => "HENRI FARMAN",
"T2_60113_60213" => "ISSY-VAL DE SEINE",
"T2_60112_60212" => "JACQUES-HENRI LARTIGUE",
"T2_60101_60201" => "LA DEFENSE",
"T2_60105_60205" => "LES COTEAUX",
"T2_60106_60206" => "LES MILONS",
"T2_60111_60211" => "LES MOULINEAUX",
"T2_60110_60210" => "MEUDON-SUR-SEINE",
"T2_60108_60208" => "MUSEE DE SEVRES",
"T2_60107_60207" => "PARC DE SAINT-CLOUD",
"T2_545_546" => "PORTE D'ISSY",
"T2_544_551" => "PORTE DE VERSAILLES",
"T2_60102_60202" => "PUTEAUX",
"T2_60104_60204" => "SURESNES-LONGCHAMP",
"T2_547_548" => "SUZANNE LENGLEN"),
"T3" => array( "T3_686_709" => "BALARD",
"T3_715_716" => "BRANCION",
"T3_695_722" => "CITE UNIVERSITAIRE",
"T3_687_712" => "DESNOUETTES",
"T3_691_718" => "DIDOT",
"T3_689_714" => "GEORGES BRASSENS",
"T3_692_719" => "JEAN MOULIN",
"T3_694_721" => "MONTSOURIS",
"T3_685_755" => "PONT DU GARIGLIANO",
"T3_698_725" => "PORTE D'ITALIE",
"T3_702" => "PORTE D'IVRY",
"T3_693_720" => "PORTE D'ORLEANS",
"T3_699_726" => "PORTE DE CHOISY",
"T3_690_717" => "PORTE DE VANVES",
"T3_688_713" => "PORTE DE VERSAILLES",
"T3_697_724" => "POTERNE DES PEUPLIERS",
"T3_696_723" => "STADE CHARLETY"),
"RA" => array("4" => "AUBER",
"15" => "BOISSY SAINT LEGER",
"31" => "BRY SUR MARNE",
"25" => "BUSSY ST-GEORGES",
"18" => "CHAMPIGNY",
"5" => "CHARLES DE GAULLE-ETOILE",
"3" => "CHATELET-LES-HALLES",
"10" => "CHATOU-CROISSY",
"23" => "FONTENAY SOUS BOIS",
"2" => "GARE DE LYON",
"6" => "GRANDE ARCHE LA DEFENSE",
"21" => "JOINVILLE LE PONT",
"17" => "LA VARENNE-CHENNEVIERES",
"19" => "LE PARC DE SAINT MAUR",
"9" => "LE VESINET CENTRE",
"8" => "LE VESINET LE PECQ",
"27" => "LOGNES",
"24" => "MARNE LA VALLEE-CHESSY",
"34|14" => "NANTERRE PREFECTURE",
"13" => "NANTERRE UNIVERSITE",
"12" => "NANTERRE VILLE",
"1" => "NATION",
"32" => "NEUILLY-PLAISANCE",
"22" => "NOGENT SUR MARNE",
"28" => "NOISIEL",
"30" => "NOISY LE GRAND-MONT D'EST",
"29" => "NOISY-CHAMPS",
"11" => "RUEIL MALMAISON",
"20" => "SAINT MAUR-CRETEIL",
"7" => "ST GERMAIN EN LAYE",
"16" => "SUCY-BONNEUIL",
"26" => "TORCY",
"35" => "VAL D'EUROPE-SERRIS",
"33" => "VAL DE FONTENAY",
"0" => "VINCENNES"),

"RB" => array("19" => "ANTONY",
"14" => "ARCUEIL CACHAN",
"13" => "BAGNEUX",
"22|12" => "BOURG LA REINE",
"27" => "BURES SUR YVETTE",
"1" => "CHATELET",
"6" => "CITE UNIVERSITAIRE",
"24" => "COURCELLES SUR YVETTE",
"5" => "DENFERT ROCHEREAU",
"18" => "FONTAINE MICHALON",
"10" => "FONTENAY AUX ROSES",
"7" => "GENTILLY",
"25" => "GIF SUR YVETTE",
"20" => "LA CROIX DE BERNY",
"26" => "LA HACQUINIERE",
"8" => "LAPLACE",
"29" => "LE GUICHET",
"17" => "LES BACONNETS",
"30" => "LOZERE",
"3" => "LUXEMBOURG",
"15" => "MASSY PALAISEAU",
"16" => "MASSY VERRIERES",
"28" => "ORSAY VILLE",
"32" => "PALAISEAU",
"31" => "PALAISEAU VILLEBON",
"21" => "PARC DE SCEAUX",
"4" => "PORT ROYAL",
"9" => "ROBINSON",
"2" => "SAINT MICHEL",
"23" => "SAINT REMY LES CHEVREUSE",
"11" => "SCEAUX"),
"1" => array("argentine" => "Argentine",
"bastille" => "Bastille",
"berault" => "Bérault",
"champs elysees clemenceau" => "Champs-Elysées Clémenceau",
"charles de gaulle etoile" => "Charles de Gaulle Etoile",
"chateau de vincennes" => "Château de Vincennes",
"chatelet" => "Châtelet",
"concorde" => "Concorde",
"esplanade de la defense" => "Esplanade de La Défense",
"franklin d roosevelt" => "Franklin D. Roosevelt",
"gare de lyon" => "Gare de Lyon",
"george v" => "George V",
"hotel de ville" => "Hôtel de Ville",
"la defense" => "La Défense",
"les sablons" => "Les Sablons",
"louvre rivoli" => "Louvre - Rivoli",
"nation" => "Nation",
"palais royal musee du louvre" => "Palais Royal Musée du Louvre",
"pont de neuilly" => "Pont de Neuilly",
"porte maillot" => "Porte Maillot",
"porte de vincennes" => "Porte de Vincennes",
"reuilly diderot" => "Reuilly-Diderot",
"saint mande" => "Saint-Mandé",
"saint paul" => "Saint-Paul",
"tuileries" => "Tuileries"),
"2" => array( "alexandre dumas" => "Alexandre Dumas",
"anvers" => "Anvers",
"avron" => "Avron",
"barbes rochechouart" => "Barbès Rochechouart",
"belleville" => "Belleville",
"blanche" => "Blanche",
"charles de gaulle etoile" => "Charles de Gaulle Etoile",
"colonel fabien" => "Colonel Fabien",
"courcelles" => "Courcelles",
"couronnes" => "Couronnes",
"jaures" => "Jaurès",
"la chapelle" => "La Chapelle",
"monceau" => "Monceau",
"menilmontant" => "Ménilmontant",
"nation" => "Nation",
"philippe auguste" => "Philippe Auguste",
"pigalle" => "Pigalle",
"place de clichy" => "Place de Clichy",
"porte dauphine" => "Porte Dauphine",
"pere lachaise" => "Père Lachaise",
"rome" => "Rome",
"stalingrad" => "Stalingrad",
"ternes" => "Ternes",
"victor hugo" => "Victor Hugo",
"villiers" => "Villiers"),
"3" => array( "anatole france" => "Anatole France",
"arts et metiers" => "Arts et Métiers",
"bourse" => "Bourse",
"europe" => "Europe",
"gallieni" => "Gallieni",
"gambetta" => "Gambetta",
"havre caumartin" => "Havre-Caumartin",
"louise michel" => "Louise Michel",
"malesherbes" => "Malesherbes",
"opera" => "Opéra",
"parmentier" => "Parmentier",
"pereire" => "Pereire",
"pont de levallois becon" => "Pont de Levallois Bécon",
"porte de bagnolet" => "Porte de Bagnolet",
"porte de champerret" => "Porte de Champerret",
"pere lachaise" => "Père Lachaise",
"quatre septembre" => "Quatre-Septembre",
"rue saint maur" => "Rue Saint-Maur",
"reaumur sebastopol" => "Réaumur-Sébastopol",
"republique" => "République",
"saint lazare" => "Saint-Lazare",
"sentier" => "Sentier",
"temple" => "Temple",
"villiers" => "Villiers",
"wagram" => "Wagram"),
"4" => array( "alesia" => "Alésia",
"barbes rochechouart" => "Barbès Rochechouart",
"chateau rouge" => "Château Rouge",
"chateau d eau" => "Château d'Eau",
"chatelet" => "Châtelet",
"cite" => "Cité",
"denfert rochereau" => "Denfert-Rochereau",
"etienne marcel" => "Etienne Marcel",
"gare de l est" => "Gare de l'Est",
"gare du nord" => "Gare du Nord",
"les halles" => "Les Halles",
"marcadet poissonniers" => "Marcadet Poissonniers",
"montparnasse bienvenue" => "Montparnasse BienvenÃ¼e",
"mouton duvernet" => "Mouton-Duvernet",
"odeon" => "Odéon",
"porte d orleans" => "Porte d'Orléans",
"porte de clignancourt" => "Porte de Clignancourt",
"raspail" => "Raspail",
"reaumur sebastopol" => "Réaumur-Sébastopol",
"saint germain des pres" => "Saint-Germain-des-Prés",
"saint michel" => "Saint-Michel",
"saint placide" => "Saint-Placide",
"saint sulpice" => "Saint-Sulpice",
"simplon" => "Simplon",
"strasbourg saint denis" => "Strasbourg-Saint-Denis",
"vavin" => "Vavin"),
"5" => array( "bastille" => "Bastille",
"bobigny pablo picasso" => "Bobigny Pablo Picasso",
"bobigny pantin raymond queneau" => "Bobigny-Pantin Raymond Queneau",
"breguet sabin" => "Bréguet-Sabin",
"campo formio" => "Campo-Formio",
"eglise de pantin" => "Eglise de Pantin",
"gare d austerlitz" => "Gare d'Austerlitz",
"gare de l est" => "Gare de l'Est",
"gare du nord" => "Gare du Nord",
"hoche" => "Hoche",
"jacques bonsergent" => "Jacques Bonsergent",
"jaures" => "Jaurès",
"laumiere" => "Laumière",
"oberkampf" => "Oberkampf",
"ourcq" => "Ourcq",
"place d italie" => "Place d'Italie",
"porte de pantin" => "Porte de Pantin",
"quai de la rapee" => "Quai de la Rapée",
"richard lenoir" => "Richard-Lenoir",
"republique" => "République",
"saint marcel" => "Saint-Marcel",
"stalingrad" => "Stalingrad"),
"6" => array( "bel air" => "Bel-Air",
"bercy" => "Bercy",
"bir hakeim" => "Bir-Hakeim",
"boissiere" => "Boissière",
"cambronne" => "Cambronne",
"charles de gaulle etoile" => "Charles de Gaulle Etoile",
"chevaleret" => "Chevaleret",
"corvisart" => "Corvisart",
"daumesnil" => "Daumesnil",
"denfert rochereau" => "Denfert-Rochereau",
"dugommier" => "Dugommier",
"dupleix" => "Dupleix",
"edgar quinet" => "Edgar Quinet",
"glaciere" => "Glacière",
"kleber" => "Kléber",
"la motte picquet grenelle" => "La Motte-Picquet Grenelle",
"montparnasse bienvenue" => "Montparnasse BienvenÃ¼e",
"nation" => "Nation",
"nationale" => "Nationale",
"passy" => "Passy",
"pasteur" => "Pasteur",
"picpus" => "Picpus",
"place d italie" => "Place d'Italie",
"quai de la gare" => "Quai de la Gare",
"raspail" => "Raspail",
"saint jacques" => "Saint-Jacques",
"sevres lecourbe" => "Sèvres-Lecourbe",
"trocadero" => "Trocadéro"),
"7" => array( "aubervilliers pantin quatre chemins" => "Aubervilliers Pantin Quatre Chemins",
"cadet" => "Cadet",
"censier daubenton" => "Censier Daubenton",
"chaussee d antin la fayette" => "Chaussée d'Antin La Fayette",
"chateau landon" => "Château-Landon",
"chatelet" => "Châtelet",
"corentin cariou" => "Corentin Cariou",
"crimee" => "Crimée",
"fort d aubervilliers" => "Fort d'Aubervilliers",
"gare de l est" => "Gare de l'Est",
"jussieu" => "Jussieu",
"la courneuve 8 mai 1945" => "La Courneuve 8 mai 1945",
"le kremlin bicetre" => "Le Kremlin-Bicétre",
"le peletier" => "Le Peletier",
"les gobelins" => "Les Gobelins",
"louis blanc" => "Louis Blanc",
"mairie d ivry" => "Mairie d'Ivry",
"maison blanche" => "Maison Blanche",
"opera" => "Opéra",
"palais royal musee du louvre" => "Palais Royal Musée du Louvre",
"pierre et marie curie" => "Pierre et Marie Curie",
"place monge" => "Place Monge",
"place d italie" => "Place d'Italie",
"poissonniere" => "Poissonnière",
"pont marie" => "Pont Marie",
"pont neuf" => "Pont Neuf",
"porte d italie" => "Porte d'Italie",
"porte d ivry" => "Porte d'Ivry",
"porte de choisy" => "Porte de Choisy",
"porte de la villette" => "Porte de la Villette",
"pyramides" => "Pyramides",
"riquet" => "Riquet",
"stalingrad" => "Stalingrad",
"sully morland" => "Sully-Morland",
"tolbiac" => "Tolbiac",
"villejuif louis aragon" => "Villejuif Louis Aragon",
"villejuif leo lagrange" => "Villejuif Léo Lagrange",
"villejuif paul vaillant couturier" => "Villejuif Paul Vaillant-Couturier"),
"8" => array( "balard" => "Balard",
"bastille" => "Bastille",
"bonne nouvelle" => "Bonne Nouvelle",
"boucicaut" => "Boucicaut",
"charenton ecoles" => "Charenton-Ecoles",
"chemin vert" => "Chemin Vert",
"commerce" => "Commerce",
"concorde" => "Concorde",
"creteil l echat" => "Créteil-L'Echat",
"creteil prefecture" => "Créteil-Préfecture",
"creteil universite" => "Créteil-Université",
"daumesnil" => "Daumesnil",
"ecole militaire" => "Ecole Militaire",
"ecole veterinaire de maisons alfort" => "Ecole Vétérinaire de Maisons-Alfort",
"faidherbe chaligny" => "Faidherbe-Chaligny",
"filles du calvaire" => "Filles du Calvaire",
"felix faure" => "Félix Faure",
"grands boulevards" => "Grands Boulevards",
"invalides" => "Invalides",
"la motte picquet grenelle" => "La Motte-Picquet Grenelle",
"la tour maubourg" => "La Tour-Maubourg",
"ledru rollin" => "Ledru-Rollin",
"liberte" => "Liberté",
"lourmel" => "Lourmel",
"madeleine" => "Madeleine",
"maisons alfort les juilliottes" => "Maisons-Alfort Les Juilliottes",
"maisons alfort stade" => "Maisons-Alfort Stade",
"michel bizot" => "Michel Bizot",
"montgallet" => "Montgallet",
"opera" => "Opéra",
"porte doree" => "Porte Dorée",
"porte de charenton" => "Porte de Charenton",
"reuilly diderot" => "Reuilly-Diderot",
"richelieu drouot" => "Richelieu-Drouot",
"republique" => "République",
"saint sebastien froissart" => "Saint-Sébastien Froissart",
"strasbourg saint denis" => "Strasbourg-Saint-Denis"),
"9" => array( "alma marceau" => "Alma-Marceau",
"billancourt" => "Billancourt",
"bonne nouvelle" => "Bonne Nouvelle",
"buzenval" => "Buzenval",
"charonne" => "Charonne",
"chaussee d antin la fayette" => "Chaussée d'Antin La Fayette",
"croix de chavaux" => "Croix de Chavaux",
"exelmans" => "Exelmans",
"franklin d roosevelt" => "Franklin D. Roosevelt",
"grands boulevards" => "Grands Boulevards",
"havre caumartin" => "Havre-Caumartin",
"iena" => "Iéna",
"jasmin" => "Jasmin",
"la muette" => "La Muette",
"mairie de montreuil" => "Mairie de Montreuil",
"maraichers" => "MaraÃ®chers",
"marcel sembat" => "Marcel Sembat",
"michel ange auteuil" => "Michel-Ange Auteuil",
"michel ange molitor" => "Michel-Ange Molitor",
"miromesnil" => "Miromesnil",
"nation" => "Nation",
"oberkampf" => "Oberkampf",
"pont de sevres" => "Pont de Sèvres",
"porte de montreuil" => "Porte de Montreuil",
"porte de saint cloud" => "Porte de Saint-Cloud",
"ranelagh" => "Ranelagh",
"richelieu drouot" => "Richelieu-Drouot",
"robespierre" => "Robespierre",
"rue de la pompe" => "Rue de la Pompe",
"rue des boulets" => "Rue des Boulets",
"republique" => "République",
"saint ambroise" => "Saint-Ambroise",
"saint augustin" => "Saint-Augustin",
"saint philippe du roule" => "Saint-Philippe-du-Roule",
"strasbourg saint denis" => "Strasbourg-Saint-Denis",
"trocadero" => "Trocadéro",
"voltaire" => "Voltaire"),
"10" => array( "avenue emile zola" => "Avenue Emile Zola",
"boulogne jean jaures" => "Boulogne Jean Jaurès",
"boulogne pont de saint cloud" => "Boulogne Pont de Saint-Cloud",
"cardinal lemoine" => "Cardinal Lemoine",
"chardon lagache" => "Chardon-Lagache",
"charles michels" => "Charles Michels",
"cluny la sorbonne" => "Cluny La Sorbonne",
"duroc" => "Duroc",
"gare d austerlitz" => "Gare d'Austerlitz",
"javel andre citroen" => "Javel André CitroÃ«n",
"jussieu" => "Jussieu",
"la motte picquet grenelle" => "La Motte-Picquet Grenelle",
"mabillon" => "Mabillon",
"maubert mutualite" => "Maubert Mutualité",
"michel ange molitor" => "Michel-Ange Molitor",
"mirabeau" => "Mirabeau",
"odeon" => "Odéon",
"sevres babylone" => "Sèvres-Babylone",
"segur" => "Ségur",
"vaneau" => "Vaneau"),
"11" => array( "arts et metiers" => "Arts et Métiers",
"belleville" => "Belleville",
"chatelet" => "Châtelet",
"goncourt" => "Goncourt",
"hotel de ville" => "Hôtel de Ville",
"jourdain" => "Jourdain",
"mairie des lilas" => "Mairie des Lilas",
"place des fetes" => "Place des Fétes",
"porte des lilas" => "Porte des Lilas",
"pyrenees" => "Pyrénées",
"rambuteau" => "Rambuteau",
"republique" => "République",
"telegraphe" => "Télégraphe"),
"12" => array( "abbesses" => "Abbesses",
"assemblee nationale" => "Assemblée Nationale",
"concorde" => "Concorde",
"convention" => "Convention",
"corentin celton" => "Corentin Celton",
"falguiere" => "Falguière",
"jules joffrin" => "Jules Joffrin",
"lamarck caulaincourt" => "Lamarck Caulaincourt",
"madeleine" => "Madeleine",
"mairie d issy" => "Mairie d'Issy",
"marcadet poissonniers" => "Marcadet Poissonniers",
"marx dormoy" => "Marx Dormoy",
"montparnasse bienvenue" => "Montparnasse BienvenÃ¼e",
"notre dame de lorette" => "Notre-Dame-de-Lorette",
"455" => "Notre-Dame-des-Champs",
"pasteur" => "Pasteur",
"pigalle" => "Pigalle",
"porte de versailles" => "Porte de Versailles",
"porte de la chapelle" => "Porte de la Chapelle",
"rennes" => "Rennes",
"rue du bac" => "Rue du Bac",
"saint georges" => "Saint-Georges",
"saint lazare" => "Saint-Lazare",
"solferino" => "Solférino",
"sevres babylone" => "Sèvres-Babylone",
"vaugirard" => "Vaugirard",
"volontaires" => "Volontaires"),
"13" => array( "asnieres gennevilliers les courtilles" => "Asnières-Gennevilliers - Les Courtilles",
"basilique de saint denis" => "Basilique de Saint-Denis",
"brochant" => "Brochant",
"carrefour pleyel" => "Carrefour Pleyel",
"champs elysees clemenceau" => "Champs-Elysées Clémenceau",
"chatillon montrouge" => "Châtillon-Montrouge",
"duroc" => "Duroc",
"gabriel peri" => "Gabriel Péri",
"garibaldi" => "Garibaldi",
"gaite" => "Gaïté",
"guy moquet" => "Guy Môquet",
"invalides" => "Invalides",
"la fourche" => "La Fourche",
"les agnettes" => "Les Agnettes",
"liege" => "Liège",
"mairie de clichy" => "Mairie de Clichy",
"mairie de saint ouen" => "Mairie de Saint-Ouen",
"malakoff plateau de vanves" => "Malakoff Plateau de Vanves",
"malakoff rue etienne dolet" => "Malakoff Rue Etienne Dolet",
"miromesnil" => "Miromesnil",
"montparnasse bienvenue" => "Montparnasse Bienvenüe",
"pernety" => "Pernety",
"place de clichy" => "Place de Clichy",
"plaisance" => "Plaisance",
"porte de clichy" => "Porte de Clichy",
"porte de saint ouen" => "Porte de Saint-Ouen",
"porte de vanves" => "Porte de Vanves",
"saint denis porte de paris" => "Saint-Denis-Porte de Paris",
"saint denis universite" => "Saint-Denis-Université",
"saint francois xavier" => "Saint-Francois-Xavier",
"saint lazare" => "Saint-Lazare",
"varenne" => "Varenne"),
"14" => array( "bercy" => "Bercy",
"bibliotheque francois mitterrand" => "Bibliothèque François Mitterrand",
"chatelet" => "Châtelet",
"cour saint emilion" => "Cour Saint-Emilion",
"gare de lyon" => "Gare de Lyon",
"madeleine" => "Madeleine",
"olympiades" => "Olympiades",
"pyramides" => "Pyramides",
"saint lazare" => "Saint-Lazare"));

$Ztamps = $ojnAPI->GetListofZtamps(false);
$Langs = array('fr'=>'Francais','en'=>'Anglais');
if(!empty($_POST['a'])) {
	if($_POST['a']=="addStop" && isset($_POST['z'])) {
		if(!empty($_POST['addArret'])) {
			$item = $_POST['addReseau']."|".$_POST['addLigne']."|".$_POST['addArret']."|".$_POST['addDirection'];
			$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/addarret?item=".urlencode($item)."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=ratp");
		}
	}else if($_POST['a']=="webcast") {
		if(!empty($_POST['webcastT']) && !empty($_POST['webcastC'])) {
				$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/addwebcast?item=".urlencode($_POST['webcastC'])."&time=".$_POST['webcastT']."&".$ojnAPI->getToken());
			$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
			header("Location: bunny_plugin.php?p=ratp");
		}
	}	else if($_POST['a'] == "rfidadd") {
		//var_dump($_POST);
			if(!empty($_POST['item']) && !empty($_POST['Tag_Rfa']) && isset($Ztamps[$_POST['Tag_Rfa']])) {
				$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/addrfid?tag=".$_POST['Tag_Rfa']."&item=".urlencode($_POST['item'])."&".$ojnAPI->getToken());
				$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
				header("Location: bunny_plugin.php?p=ratp");
			}
		} else if($_POST['a'] == "rfidd") {
			if(!empty($_POST['Tag_Rf']) && isset($Ztamps[$_POST['Tag_Rf']])) {
				$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/removerfid?tag=".$_POST['Tag_Rf']."&".$ojnAPI->getToken());
				$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
				header("Location: bunny_plugin.php?p=ratp");
			}
		}
}
else if(!empty($_GET['ra'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/removearret?item=".urlencode($_GET['ra'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	//header("Location: bunny_plugin.php?p=ratp");
}
else if(!empty($_GET['d'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/setdefaultarret?item=".urlencode($_GET['d'])."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=ratp");
}
else if(!empty($_GET['rw'])) {
	$retour = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/removewebcast?time=".$_GET['rw']."&".$ojnAPI->getToken());
	$_SESSION['message'] = isset($retour['ok']) ? $retour['ok'] : "Error : ".$retour['error'];
	header("Location: bunny_plugin.php?p=ratp"); 
}
$default = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/getdefaultarret?".$ojnAPI->getToken());
$default = isset($default['value']) ? (string)($default['value']) : '';
$pList = $ojnAPI->getApiList("bunny/".$_SESSION['bunny']."/ratp/getlistarret?".$ojnAPI->getToken());
$wList = $ojnAPI->getApiString("bunny/".$_SESSION['bunny']."/ratp/getwebcastslist?".$ojnAPI->getToken());

?>
<form method="post">
<fieldset>
<legend>Ajouter un arrêt</legend>
Réseau : <select name="addReseau">
<?php
foreach($reseau as $a => $value){
		echo "<option name=\"".$a."\" ".($a==$_POST['addReseau']?"selected=\"selected\"":"").">".$a."</option>";
}
?>
</select>
<input type="submit" value="refresh" name="a"  />
<br />
Ligne : 
<?php
if(isset($_POST['addReseau']) && $_POST['addReseau'] != ""){
	echo "<select name=\"addLigne\">";
	foreach($reseau[$_POST['addReseau']] as $a => $value){
		echo "<option name=\"".$value."\" ".($value==$_POST['addLigne']?"selected=\"selected\"":"").">".$value."</option>";
	}
	echo '</select><input type="submit" value="refresh" /><br />';
}
else{
?>
<input type="text" name="addLigne"><br />
<?php
}
?>
Arrêt
<?php
if(isset($_POST['addReseau']) && isset($_POST['addLigne']) && $_POST['addLigne'] != ""){
	echo "<select name=\"addArret\">";
	foreach($arret[$_POST['addLigne']] as $a => $value){
		echo "<option value=\"".$a."\">".$value."</option>";		
	}
	echo '</select><br />';
}
else{
?>
<input type="text" name="addArret"><br />
<?php
}
?>
Direction : 
<?php
if(isset($_POST['addReseau']) && isset($_POST['addLigne']) && $_POST['addLigne'] != ""){
	echo "<select name=\"addDirection\">";
	foreach($direction[$_POST['addLigne']] as $a => $value){
		echo "<option value=\"".$a."\">".$value."</option>";		
	}
	echo '</select><br />';
}
else{
?>
<input type="text" name="addArret"><br />
<?php
}
if(isset($_POST['addReseau']) && isset($_POST['addLigne']) && $_POST['addLigne'] != ""){
	echo '<input type="hidden" name="a" value="addStop">';
	echo '<input type="submit" name ="z" value="Ajouter" />';
}
?>
<br /><br />
</fieldset>
</form>
<form method="post">
<fieldset>
<legend>Actions</legend>
<input type="radio" name="a" value="webcast" /> Ajouter un webcast (hh:mm) <input type="text" name="webcastT" maxlength="5" style="width:50px" /> pour l'arrêt <select name="webcastC">
	<option value=""></option>
	<?php if(!empty($pList))
	foreach($pList as $item) { ?>
		<option value="<?php echo urldecode($item) ?>"><?php echo urldecode($item); ?></option>
	<?php } ?>
</select><br />
<input type="radio" name="a" value="rfidadd" /> Ajouter une action RFID pour <select name="item">
	<option value=""></option>
	<?php  if(!empty($pList))
	foreach($pList as $item) { 
		$tmp = explode("|", urldecode($item));
		?>
		<option value="<?php echo urldecode($item) ?>"><?php echo $tmp[0]." - ".$tmp[1]." - ".$arret[$tmp[1]][$tmp[2]]." - ".$direction[$tmp[1]][$tmp[3]]; ?></option>
	<?php } ?>
</select> avec le Ztamp: <select name="Tag_Rfa">
    <option value=""></option>
	<?php foreach($Ztamps as $k=>$v): ?>
	<option value="<?php echo $k; ?>"><?php echo $v; ?> (<?php echo $k; ?>)</option>
	<?php endforeach; ?>
	</select><br />
	<input type="radio" name="a" value="rfidd" /> Supprimer l'association avec le ZTamp: <select name="Tag_Rf">
    <option value=""></option>
	<?php foreach($Ztamps as $k=>$v): ?>
	<option value="<?php echo $k; ?>"><?php echo $v; ?> (<?php echo $k; ?>)</option>
	<?php endforeach; ?>
	</select><br />
<input type="submit" value="Enregister">

<?php
if(!empty($pList)) {
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="6">Les arrets</th>
	</tr>
	<tr>
		<th>Réseau</th>				
		<th>Ligne</th>
		<th>Arret</th>
		<th>Direction</th>
		<th colspan="2">Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($pList as $item) {
		$tmp = explode("|", urldecode($item));
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo $tmp[0]; ?></td>
		<td><?php echo $tmp[1]; ?></td>
		<td><?php echo $arret[$tmp[1]][$tmp[2]]; ?></td>
		<td><?php echo $direction[$tmp[1]][$tmp[3]]; ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=ratp&ra=<?php echo $item ?>">Remove</a></td>
		<td width="15%"><?php if($default != $item) { ?><a href="bunny_plugin.php?p=ratp&d=<?php echo $item ?>">Set as default</a><?php } else { ?>Default Stop<?php } ?></td>
	</tr>
<?php } ?>
</table>
<?php
}
if(isset($wList['list']->item)){
?>
<hr />
<center>
<table style="width: 80%">
	<tr>
		<th colspan="3">Webcast</th>
	</tr>
	<tr>
		<th>Time</th>
		<th>Name</th>
		<th>Actions</th>
	</tr>
<?php
	$i = 0;
	foreach($wList['list']->item as $item) {
?>
	<tr<?php echo $i++ % 2 ? " class='l2'" : "" ?>>
		<td><?php echo urldecode($item->key) ?></td>
		<td><?php echo $item->value ?></td>
		<td width="15%"><a href="bunny_plugin.php?p=ratp&rw=<?php echo $item->key ?>">Remove</a></td>
	</tr>
<?php  } ?>
</table>
<?php } ?>
</fieldset>
</form>
