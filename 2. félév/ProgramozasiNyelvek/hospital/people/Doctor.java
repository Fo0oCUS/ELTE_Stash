package hospital.people;

import java.util.*;

import hospital.utils.IllegalDataException;
import hospital.utils.Insurance;

public class Doctor extends Patient implements Medicinal{
    private LinkedList<Patient> patients = new LinkedList<>();
    
    public Doctor(int id, String name, int age, Insurance ins) throws IllegalDataException{
        super(id, name, age, ins);
        setPriority(10);
        setAttendant(null);
    }

    public int waitingList(){ return patients.size(); }

    public void admit(Patient patient){ 
        boolean admitted = false;
        int index = 0;
        for(int i = 0; i < waitingList(); i++){
            if(patient.compareTo( patients.get(i) ) > 0){
                index = i;
                admitted = true;
                break;
            }
        }
        if(admitted){
            patients.add(index, patient);
        }
        else{
            patients.add(patient);
        }
    }

    public void treat(){ patients.removeFirst(); }

    public Patient discharge(String name){
        int index = 0;
        for(int i = 0; i < waitingList(); i++){
            if(patients.get(i).name == name){
                index = i;
                break;
            }
        }
        Patient p = patients.get(index);
        patients.remove(index);
        return p;
    }

    @Override
    public String toString(){
        StringBuilder sb = new StringBuilder();

        sb.append(super.toString());

        for (Patient patient : patients) {
            sb.append("\n");
            sb.append(patient.toString());
        }

        return sb.toString();
    }
}
