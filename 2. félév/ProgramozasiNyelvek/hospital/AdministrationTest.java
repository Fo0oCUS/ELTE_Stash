package hospital;

import hospital.utils.*;
import hospital.people.*;
import java.io.FileNotFoundException;

import static check.CheckThat.*;
import static org.junit.jupiter.api.Assertions.*;
import org.junit.jupiter.api.*;
import org.junit.jupiter.api.condition.*;
import org.junit.jupiter.api.extension.*;
import org.junit.jupiter.params.*;
import org.junit.jupiter.params.provider.*;
import check.*;

public class AdministrationTest {
    @Test
    public void CountOfDoctorsIsCorrect() throws FileNotFoundException, IllegalDataException{
        Administration a = new Administration("hospitalData.txt");
        assertEquals(10, a.getMedicalpersonnel().size());
    }
    
    @Test
    public void CountOfPatientsIsCorrect() throws FileNotFoundException, IllegalDataException{
        Administration a = new Administration("hospitalData.txt");
        int sum = 0;
        for( Doctor doc : a.getMedicalpersonnel().values()){
            sum += doc.waitingList();
        }
        assertEquals(108, sum);
    }
    @Test
    public void DoctorGergoryHouseTreats3Patients() throws FileNotFoundException, IllegalDataException{
        Administration a = new Administration("hospitalData.txt");
        Doctor doc = a.getMedicalpersonnel().get("Dr. Gregory House");
        doc.treat();
        doc.treat();
        doc.treat();
        assertEquals(14, doc.waitingList());
    }

    @Test
    public void SwapPaitent() throws FileNotFoundException, IllegalDataException{
        Administration a = new Administration("hospitalData.txt");
        Doctor james = a.getMedicalpersonnel().get("Dr. James Wilson");
        Doctor erica = a.getMedicalpersonnel().get("Dr. Eric Foreman");

        System.out.println(james.toString());

        Patient carmen = james.discharge("Carmen Electra");
        erica.admit(carmen);

        assertEquals(14, james.waitingList());
        assertEquals(8, erica.waitingList());
    }


}
