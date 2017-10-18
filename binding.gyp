{
    "targets": [
        {
            "target_name": "sga_fwbro",
            "sources": [ "src/main.cpp", "src/sga_fwbro.cpp", "src/fwbro.cpp", "src/slotslogic.cpp", "src/proportion.cpp" ],
            "include_dirs" : [
 	 			"<!(node -e \"require('nan')\")"
			]
        }
    ],
}