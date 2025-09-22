package hospital.people;

import hospital.utils.IllegalDataException;
import hospital.utils.Insurance;

public abstract class Person {
    
    protected final int id;
    public int getId() { return id; }

    protected final String name;
    public String getName() { return name; }

    protected int age;
    public int getAge() { return age; }
    public void setAge(int age) throws IllegalDataException{
        if(age >= 0 && age <= 125){
            this.age = age;
        }
        else{
            throw new IllegalDataException(name + " age: " + age);
        }
    }

    protected Insurance type;
    public Insurance getType(){ return type; }
    public void setType(Insurance ins){ type = ins; }

    public Person(int id, String name, int age, Insurance ins) throws IllegalDataException{
        this.id = id;
        this.name = name;
        setAge(age);
        this.type = ins;
    }

    @Override
    public String toString(){
        return "ID: " + id + "\tName: " + name + "\tAge: " + age + "\tInsurance: " + type.toString();
    }
}
