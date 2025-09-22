package hospital;

import static check.CheckThat.*;
import static check.CheckThat.Condition.*;
import org.junit.jupiter.api.*;
import check.CheckThat;

public class InsuranceStructureTest {
    @BeforeAll
    public static void init() {
        CheckThat.theEnum("hospital.utils.Insurance")
            .thatIs(FULLY_IMPLEMENTED, INSTANCE_LEVEL, VISIBLE_TO_ALL)
            .hasEnumElements("NONE", "BASIC", "ADVANCED")
            .thatHas(TEXTUAL_REPRESENTATION);
    }
}
