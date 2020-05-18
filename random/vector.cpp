

class IntVector {
private:
    int *values;
    unsigned int length;
    unsigned int capacity;
public:
    IntVector(unsigned int _length) : length(_length) {
        // Set the capacity to the lowest power of two greater than or equal to the length.
        int highest = -1;
        for (int i = sizeof(unsigned int) - 1; i >= 0; --i) {
            if (highest < 0) {
                if ((length & (1 << i)) != 0) highest = i;
            } else if ((length & (1 << i)) != 0) {
	        highest ++;
	        break;
            }
        }
        if (length == 0) highest = 0;
        capacity = 1 << highest;
        values = new int[capacity];
    };

    double& operator[](int index) {
        if (index < 0 || index >= length) {

        }
        return values[index];
    }
};

