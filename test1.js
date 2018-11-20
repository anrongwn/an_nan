'use stict';

const util = require('util');


let Base = function (id, name) {
    this.id_ = id;
    this.name_ = name;
}

Base.prototype.getId = function () {
    console.log('Base');
    console.log(`id=${this.id_}  name=${this.name_}`);
};

let Sub = function (price) {
    this.price_ = 1000;
    Base.call(this, 1, 'www');
}

util.inherits(Sub, Base);

var b_obj = new Base(0,'wangjr');
var s_obj = new Sub(9000);
s_obj.getId();
console.log(util.inspect(s_obj));
console.log(util.inspect(b_obj));
console.log(s_obj instanceof Sub);


let Demo = function (name) {
    this.name_ = name;

    if (!(this instanceof Demo)) {
        return new Demo(name);
    }
};
Demo.prototype = {
    getName: function () {
        console.log(this.name_);
    }
};

var de = Demo('wangjr----');
de.getName();

var de2 = new Demo('wangjr2----');
de2.getName();

var Car = function () { };
Car.prototype = {
    getPrice: function () {
        throw new Error('no non noon');
    }
};

let car1 = new Car();

try {
    car1.getPrice();
} catch (err){
    console.error(err);
}
//////////////////////////////////////////////////////////////
var VehicleFactory = function (subType, superType) {
    if ((typeof VehicleFactory[superType]) === 'function') {
        function F() { };
        F.prototype = new VehicleFactory[superType]();
        subType.constructor = subType;
        subType.prototype = new F();
    } else {
        throw new Error('no abstract class');
    }
};

VehicleFactory.Car = function () {
    this.type = 'car';
}
VehicleFactory.Car.prototype = {
    getPrice: function () {
        return new Error('dont user');
    }
};
VehicleFactory.Bus = function () {
    this.type = 'bus';
}
VehicleFactory.Bus.prototype = {
    getPrice: function () {
        return new Error('dont user');
    }
};

var BMW = function (price, speed) {
    this.price = price;
    this.speed = speed;
}
VehicleFactory(BMW, 'Car');
BMW.prototype.getPrice = function () {
    console.log(this.type);
    console.log(`BWM 's price : ${this.price}`);
}

let bmw = new BMW(300000, 300);
bmw.getPrice();

///////////////////////////////
var Human = function (param) {
    this.skill = param && param.skill || '保密';
    this.hobby = param && param.hobby || '保密';
}
Human.prototype = {
    getSkill: function () {
        return this.skill;
    },
    getHobby: function () {
        return this.hobby;
    }
};

var Named = function (name) {
    let that = this;
    (function (name, that) {
        that.wholeName = name;
        if (name.indexOf(' ') > -1) {
            that.firstName = name.slice(0, name.indexOf(' '));
            that.secondName = name.slice(name.indexOf(' '));
        }
    })(name, that);
};

var Work = function (work) {
    let that = this;
    (function (work, that) {
        switch (work) {
            case 'code':
                that.work = 'code';
                that.workDescript = 'coding...coding';
                break;
            case 'ui':
            case 'ue':
                that.work = 'ui';
                that.workDescript = 'user interface';
                break;
            default:
                that.work = work;
                that.workDescript = 'unknow';
                break;
        }

    })(work, that);
}
Work.prototype = {
    changeWork: function (work) {
        this.work = work;
    },
    changeDescript: function (text) {
        this.workDescript = text;
    }
};

var Person = function (name, work) {
    var _person = new Human();
    _person.name = new Named(name);
    _person.work = new Work(work);
    return _person;
};

var per = new Person('wang jianrong', 'code');
console.log(per.getSkill());
console.log(per.name.firstName);
console.log(per.name.secondName);
console.log(per.work.workDescript);
per.work.changeDescript('nothing...');
console.log(per.work.workDescript);

var Configer = (function () {
    let _conf = {
        Max: 10000,
        Min: 10,
        Count: 20000
    };

    return {
        get: function (name) {
            return _conf[name] ? _conf[name] : null;
        }
    }
})();

var count = Configer.get('Count');
console.log(count);

//////////////////////////////
var LazySingle = (function () {
    var _instance = null;

    function Single() {
        return {
            _name: 'wangjr-single',
            getName: function () {
                return this._name;
            },
            setName: function (name) {
                this._name = name;
            }
        };
    };

    return function () {
        if (!_instance) {
            _instance = Single();

            console.log(_instance.getName());
        }
        return _instance;
    }
})();

console.log(LazySingle().getName());
LazySingle().setName('wnagjr-single2')
console.log(LazySingle().getName());

