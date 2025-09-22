package hospital.people;

import hospital.utils.IllegalDataException;
import hospital.utils.Insurance;

public class Patient extends Person implements Comparable<Patient>{
    
    protected int priority;
    public int getPriority(){ return priority; }
    public void setPriority(int prio) throws IllegalDataException{
        if(prio >= 1 && prio <= 10){
            priority = prio;
        }
        else{
            throw new IllegalDataException(name + " priority: " + prio);
        }
    }

    protected String attendant;
    public String getAttendant(){ return attendant; }
    public void setAttendant(String attend){ attendant = attend; }

    public Patient(int id, String name, int age, Insurance ins, int priority, String attendant) throws IllegalDataException{
        super(id, name, age, ins);
        setPriority(priority);
        setAttendant(attendant);
    }
    public Patient(int id, String name, int age, Insurance ins) throws IllegalDataException{
        super(id, name, age, ins);
    }

    @Override
    public String toString(){
        return super.toString() + "\tPriority: " + priority + "\tAttendant: " + attendant;
    }

    @Override
    public int compareTo(Patient other){
        if( other == this ) return 0;
        if( priority > other.priority) return 1;
        if( priority < other.priority) return -1;
        if( type.ordinal() > other.type.ordinal()) return 1;
        if( type.ordinal() < other.type.ordinal()) return -1;
        if( age > other.age) return 1;
        if( age < other.age) return -1;
        return 0; // attendant várólisátjának a sorrendjében
    }

    @Override
    public int hashCode(){
        return java.util.Objects.hash(id, name, age, type, priority, attendant);
    }

    @Override
    public boolean equals(Object a){
        if( a instanceof Person ){
            if(id == ((Person)a).getId()){
                return true;
            }
        }
        return false;
    }
}
