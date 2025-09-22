package hospital.people;

public interface Medicinal {
    public void admit(Patient patient);
    public void treat();
    public Patient discharge(String name);
}
