package hospital.utils;

import java.util.*;
import java.io.*;
import hospital.people.Doctor;
import hospital.people.Patient;

public class Administration {
    private HashMap<String, Doctor> medicalpersonnel = new HashMap<>();
    public HashMap<String, Doctor> getMedicalpersonnel(){ return medicalpersonnel; }

    private LinkedList<Patient> patients = new LinkedList<>();

    public Administration(String filename) throws FileNotFoundException, IllegalDataException{
        try{
            File file = new File(filename);
            Scanner reader = new Scanner(file);
            if(reader.hasNextLine()) { reader.nextLine(); }
            while(reader.hasNextLine()){
                String line = reader.nextLine();
                String[] seged = line.split(";");
                int id = Integer.parseInt(seged[0]);
                String name = seged[1];
                int age = Integer.parseInt(seged[2]);
                Insurance ins;
                switch(seged[3]){
                    case "BASIC": ins = Insurance.BASIC; break;
                    case "ADVANCED": ins = Insurance.ADVANCED; break;
                    default: ins = Insurance.NONE;
                }
                
                if(seged.length <= 4){
                    Doctor doc = new Doctor(id, name, age, ins);
                    medicalpersonnel.put(doc.getName(), doc);
                }
                else{
                    int priority = Integer.parseInt(seged[4]);
                    String attendant = seged[5];
                    try{
                        Patient patient = new Patient(id, name, age, ins, priority, attendant);
                        patients.add(patient);
                    }
                    catch(IllegalDataException e){
                        System.out.println(e.getMessage());
                    }
                }
            }
            reader.close();
        }
        catch(FileNotFoundException e){
            System.out.println(filename + " not found!");
        }
        catch(IllegalDataException e){
            System.out.println(e.getMessage());
        }
        
        for(Patient p : patients){
            Doctor doc = medicalpersonnel.get(p.getAttendant());
            doc.admit(p);
        }
    }
}
