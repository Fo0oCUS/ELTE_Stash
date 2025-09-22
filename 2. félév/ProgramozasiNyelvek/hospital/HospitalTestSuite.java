package hospital;

import org.junit.platform.suite.api.SelectClasses;
import org.junit.platform.suite.api.Suite;

@Suite
@SelectClasses({
    PersonStructureTest.class,
    PatientStructureTest.class,
    DoctorStructureTest.class,

    MedicinalStructureTest.class,

    InsuranceStructureTest.class,
    IllegalDataExceptionStructureTest.class,

    AdministrationStructureTest.class,

    hospital.utils.AdministrationTest.class

})
public class HospitalTestSuite {}
